#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure for a contact
typedef struct Contact {
    char name[50];
    char phone[15];
    struct Contact* prev;
    struct Contact* next;
} Contact;

// creating a new contact
Contact* createContact(const char* name, const char* phone) {
    Contact* newContact = (Contact*)malloc(sizeof(Contact));
    if (newContact != NULL) {
        strcpy(newContact->name, name);
        strcpy(newContact->phone, phone);
        newContact->prev = NULL;
        newContact->next = NULL;
    }
    return newContact;
}

// pushing (insert at the beginning) a contact into the phone book
void push(Contact** head, const char* name, const char* phone, const char* filename) {
    Contact* newContact = createContact(name, phone);
    if (newContact == NULL) {
        printf("Memory allocation failed. Contact could not be added.\n");
        return;
    }

    if (*head == NULL) {
        *head = newContact;
    } else {
        newContact->next = *head;
        (*head)->prev = newContact;
        *head = newContact;
    }

    // Write contact to text file
    FILE* file = fopen(filename, "a"); // Open in append mode
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }
    fprintf(file, "%s %s\n", newContact->name, newContact->phone);
    fclose(file);

    printf("New contact added successfully.\n");
}

// pulling (remove from the beginning) a contact from the phone book
void pull(Contact** head, const char* name) {
    if (*head == NULL) {
        printf("Phone book is empty. Cannot delete.\n");
        return;
    }

    Contact* current = *head;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            if (current->prev == NULL) {
                *head = current->next;
            } else {
                current->prev->next = current->next;
            }
            if (current->next != NULL) {
                current->next->prev = current->prev;
            }
            printf("Ouch! '%s' has been removed :(\n", name);
            free(current);
            return;
        }
        current = current->next;
    }

    printf("Contact '%s' not found.\n", name);
}

// display all contacts in the phone book
void displayContacts(Contact* head) {
    if (head == NULL) {
        printf("Phone book is empty.\n");
        return;
    }
    Contact* current = head;
    printf("Contacts in the phone book:\n");
    while (current != NULL) {
        printf("Name: %s\nPhone: %s\n---------------------------------\n", current->name, current->phone);
        current = current->next;
    }
}

// free memory allocated for the phone book
void freePhoneBook(Contact* head) {
    Contact* current = head;
    while (current != NULL) {
        Contact* temp = current;
        current = current->next;
        free(temp);
    }
}

// load contacts from the text file
Contact* loadContactsFromFile(const char* filename) {
    FILE* file = fopen(filename, "a+"); // Open in append mode, create if not exist
    if (file == NULL) {
        printf("Error opening file for reading.\n");
        return NULL;
    }

    fclose(file); // Close the file

    file = fopen(filename, "r"); // Reopen in read mode
    if (file == NULL) {
        printf("Error opening file for reading.\n");
        return NULL;
    }

    Contact* head = NULL;
    char name[50];
    char phone[15];
    while (fscanf(file, "%s %s", name, phone) == 2) {
        push(&head, name, phone, filename); // Write to file while loading
    }

    fclose(file);
    printf("Contacts loaded from %s\n", filename);
    return head;
}

int main() {
    Contact* phoneBook = NULL;
    int choice;
    char name[50];
    char phone[15];

    // Load contacts from the text file
    phoneBook = loadContactsFromFile("phone.txt");

    do {
        printf("\n^.^.^.^.^.^Phone Book Menu^.^.^.^.^.^\n");
        printf("1. Create Contact\n");
        printf("2. Remove Contact\n");
        printf("3. Display Contacts\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter name: ");
                scanf("%s", name);
                printf("Enter phone number: ");
                scanf("%s", phone);
                push(&phoneBook, name, phone, "phone.txt");
                break;
            case 2:
                printf("Enter name of contact to delete: ");
                scanf("%s", name);
                pull(&phoneBook, name);
                break;
            case 3:
                displayContacts(phoneBook);
                break;
            case 4:
                printf("Exiting... Did you call your parents today? :)\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 4);

    // Freeing memory
    freePhoneBook(phoneBook);

    return 0;
}
