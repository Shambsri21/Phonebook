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

// Function to create a new contact
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

// Function to push (insert at the beginning) a contact into the phone book
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

// Function to pull (remove from the beginning) a contact from the phone book
void pull(Contact** head, const char* name, const char* filename) {
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

            // Remove contact from text file
            FILE* file = fopen(filename, "r");
            if (file == NULL) {
                printf("Error opening file for reading.\n");
                return;
            }

            FILE* tempFile = fopen("temp.txt", "w"); // Create temporary file
            if (tempFile == NULL) {
                printf("Error creating temporary file.\n");
                fclose(file);
                return;
            }

            char tempName[50];
            char tempPhone[15];
            while (fscanf(file, "%s %s", tempName, tempPhone) == 2) {
                if (strcmp(tempName, name) != 0) {
                    fprintf(tempFile, "%s %s\n", tempName, tempPhone);
                }
            }

            fclose(file);
            fclose(tempFile);

            remove(filename);            // Remove the original file
            rename("temp.txt", filename); // Rename temp file to original file

            return;
        }
        current = current->next;
    }

    printf("Contact '%s' not found.\n", name);
}

// Function to display all contacts in alphabetical order
void displayContacts(Contact* head) {
    if (head == NULL) {
        printf("Phone book is empty.\n");
        return;
    }

    // Count the number of contacts
    int count = 0;
    Contact* current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }

    // Create an array of contacts
    Contact** contactsArray = (Contact**)malloc(count * sizeof(Contact*));
    current = head;
    int i = 0;
    while (current != NULL) {
        contactsArray[i++] = current;
        current = current->next;
    }

    // Sort the contacts by name
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (strcmp(contactsArray[j]->name, contactsArray[j + 1]->name) > 0) {
                Contact* temp = contactsArray[j];
                contactsArray[j] = contactsArray[j + 1];
                contactsArray[j + 1] = temp;
            }
        }
    }

    // Display sorted contacts
    printf("Contacts in the phone book (sorted by name):\n");
    for (int i = 0; i < count; i++) {
        printf("Name: %s\nPhone: %s\n---------------------------------\n", contactsArray[i]->name, contactsArray[i]->phone);
    }

    // Free allocated memory
    free(contactsArray);
}

// Function to search for a contact by name or phone number
void searchContact(Contact* head, const char* query) {
    if (head == NULL) {
        printf("Phone book is empty.\n");
        return;
    }

    Contact* current = head;
    int found = 0;
    printf("Search results for '%s':\n", query);
    while (current != NULL) {
        if (strstr(current->name, query) != NULL || strstr(current->phone, query) != NULL) {
            printf("Name: %s\nPhone: %s\n---------------------------------\n", current->name, current->phone);
            found = 1;
        }
        current = current->next;
    }

    if (!found) {
        printf("No matching contacts found.\n");
    }
}

// Function to free memory allocated for the phone book
void freePhoneBook(Contact* head) {
    Contact* current = head;
    while (current != NULL) {
        Contact* temp = current;
        current = current->next;
        free(temp);
    }
}

// Function to load contacts from the text file
Contact* loadContactsFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
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
        printf("4. Search Contact\n");
        printf("5. Exit\n");
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
                pull(&phoneBook, name, "phone.txt");
                break;
            case 3:
                displayContacts(phoneBook);
                break;
            case 4:
                printf("Enter name or phone number to search: ");
                scanf("%s", name);
                searchContact(phoneBook, name);
                break;
            case 5:
                printf("Exiting... Did you call your parents today? :)\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 5);

    // Freeing memory
    freePhoneBook(phoneBook);

    return 0;
}
