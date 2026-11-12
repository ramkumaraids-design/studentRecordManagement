/*
 * Student Record Management System
 * Uses struct, arrays, and binary file storage.
 * Compile: gcc student_records.c -o student_records
 * Run:     ./student_records
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STUDENTS 100
#define NAME_LEN     50
#define SUBJECTS     3
#define DATA_FILE    "students.dat"

typedef struct {
    int   roll;
    char  name[NAME_LEN];
    float marks[SUBJECTS];
    float total;
    float percentage;
    char  grade;
} Student;

Student students[MAX_STUDENTS];
int count = 0;

/* ---------- Input helpers ---------- */

void readLine(const char *prompt, char *buf, int size) {
    printf("%s", prompt);
    if (fgets(buf, size, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }
    buf[strcspn(buf, "\n")] = '\0';
}

int readInt(const char *prompt) {
    char buf[64];
    int value;
    while (1) {
        readLine(prompt, buf, sizeof(buf));
        if (sscanf(buf, "%d", &value) == 1)
            return value;
        printf("Invalid input. Please enter a whole number.\n");
    }
}

float readMarks(const char *prompt) {
    char buf[64];
    float value;
    while (1) {
        readLine(prompt, buf, sizeof(buf));
        if (sscanf(buf, "%f", &value) == 1 && value >= 0 && value <= 100)
            return value;
        printf("Invalid marks. Enter a number between 0 and 100.\n");
    }
}

/* ---------- Core logic ---------- */

char calculateGrade(float percentage) {
    if (percentage >= 90) return 'A';
    if (percentage >= 75) return 'B';
    if (percentage >= 60) return 'C';
    if (percentage >= 40) return 'D';
    return 'F';
}

void computeResult(Student *s) {
    int i;
    s->total = 0;
    for (i = 0; i < SUBJECTS; i++)
        s->total += s->marks[i];
    s->percentage = s->total / SUBJECTS;
    s->grade = calculateGrade(s->percentage);
}

int findIndex(int roll) {
    int i;
    for (i = 0; i < count; i++)
        if (students[i].roll == roll)
            return i;
    return -1;
}

void printHeader(void) {
    printf("\n%-6s %-20s %-8s %-8s %-8s %-8s %-6s\n",
           "Roll", "Name", "Sub1", "Sub2", "Sub3", "Percent", "Grade");
    printf("-----------------------------------------------------------------------\n");
}

void printStudent(const Student *s) {
    printf("%-6d %-20s %-8.2f %-8.2f %-8.2f %-8.2f %-6c\n",
           s->roll, s->name, s->marks[0], s->marks[1], s->marks[2],
           s->percentage, s->grade);
}

/* ---------- File handling ---------- */

void saveToFile(void) {
    FILE *fp = fopen(DATA_FILE, "wb");
    if (fp == NULL) {
        printf("Error: could not save data.\n");
        return;
    }
    fwrite(&count, sizeof(int), 1, fp);
    fwrite(students, sizeof(Student), count, fp);
    fclose(fp);
}

void loadFromFile(void) {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL)
        return; /* no saved data yet */
    if (fread(&count, sizeof(int), 1, fp) != 1 || count < 0 || count > MAX_STUDENTS) {
        count = 0;
    } else if (fread(students, sizeof(Student), count, fp) != (size_t)count) {
        count = 0;
    }
    fclose(fp);
}

/* ---------- Menu operations ---------- */

void addStudent(void) {
    Student s;
    int i;

    if (count >= MAX_STUDENTS) {
        printf("Record list is full.\n");
        return;
    }

    s.roll = readInt("Enter roll number: ");
    if (findIndex(s.roll) != -1) {
        printf("A student with this roll number already exists.\n");
        return;
    }

    readLine("Enter name: ", s.name, NAME_LEN);
    for (i = 0; i < SUBJECTS; i++) {
        char prompt[32];
        sprintf(prompt, "Enter marks for subject %d: ", i + 1);
        s.marks[i] = readMarks(prompt);
    }

    computeResult(&s);
    students[count++] = s;
    saveToFile();
    printf("Student added successfully.\n");
}

void displayAll(void) {
    int i;
    if (count == 0) {
        printf("No records found.\n");
        return;
    }
    printHeader();
    for (i = 0; i < count; i++)
        printStudent(&students[i]);
}

void searchStudent(void) {
    int roll = readInt("Enter roll number to search: ");
    int idx = findIndex(roll);
    if (idx == -1) {
        printf("Student not found.\n");
        return;
    }
    printHeader();
    printStudent(&students[idx]);
}

void updateStudent(void) {
    int roll = readInt("Enter roll number to update: ");
    int idx = findIndex(roll);
    int i;
    char name[NAME_LEN];

    if (idx == -1) {
        printf("Student not found.\n");
        return;
    }

    readLine("Enter new name (leave blank to keep current): ", name, NAME_LEN);
    if (name[0] != '\0')
        strcpy(students[idx].name, name);

    for (i = 0; i < SUBJECTS; i++) {
        char prompt[32];
        sprintf(prompt, "Enter new marks for subject %d: ", i + 1);
        students[idx].marks[i] = readMarks(prompt);
    }

    computeResult(&students[idx]);
    saveToFile();
    printf("Record updated successfully.\n");
}

void deleteStudent(void) {
    int roll = readInt("Enter roll number to delete: ");
    int idx = findIndex(roll);
    int i;

    if (idx == -1) {
        printf("Student not found.\n");
        return;
    }

    for (i = idx; i < count - 1; i++)
        students[i] = students[i + 1];
    count--;
    saveToFile();
    printf("Record deleted successfully.\n");
}

void sortByPercentage(void) {
    int i, j;
    Student temp;

    if (count == 0) {
        printf("No records to sort.\n");
        return;
    }

    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - i - 1; j++) {
            if (students[j].percentage < students[j + 1].percentage) {
                temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }
    saveToFile();
    printf("Records sorted by percentage (highest first).\n");
    displayAll();
}

/* ---------- Main ---------- */

int main(void) {
    int choice;

    loadFromFile();

    do {
        printf("\n===== STUDENT RECORD MANAGEMENT =====\n");
        printf("1. Add Student\n");
        printf("2. Display All Students\n");
        printf("3. Search Student\n");
        printf("4. Update Student\n");
        printf("5. Delete Student\n");
        printf("6. Sort by Percentage\n");
        printf("0. Exit\n");
        choice = readInt("Enter your choice: ");

        switch (choice) {
            case 1: addStudent();       break;
            case 2: displayAll();       break;
            case 3: searchStudent();    break;
            case 4: updateStudent();    break;
            case 5: deleteStudent();    break;
            case 6: sortByPercentage(); break;
            case 0: printf("Goodbye!\n"); break;
            default: printf("Invalid choice. Try again.\n");
        }
    } while (choice != 0);

    return 0;
}