// 1st list node
struct node {
	int reportId;
	int companyId;
	char typeReport[5] = {0};
	char nameFile[50] = {0};
	char dateFormat[20] = {0};
	struct node * next;
};
typedef struct node Node;

// DAT/TAD: Data Abstract Type Begin
// -----------------------------------------------------------------
// Initialize the list
Node * begin(){
	return NULL;
}

// Insert a new node
Node * insertNode(Node* reportList, int reportId, int companyId, char typeReport[], char nameFile[], char dateFormat[]){
	Node *new_node = (Node*) malloc(sizeof(Node));
	new_node->reportId = reportId;
	new_node->companyId = companyId;
    for(int i = 0; i < strlen(typeReport); i++)
        new_node->typeReport[i] = typeReport[i];
	for(int i = 0; i < strlen(nameFile); i++)
        new_node->nameFile[i] = nameFile[i];
    for(int i = 0; i < strlen(dateFormat); i++)
        new_node->dateFormat[i] = dateFormat[i];
	new_node->next = reportList;
	return new_node;
}

// Remove a node at begin or mid the list
Node * removeNode(Node *reportList, int id){
	Node *previous = NULL;			// Pointer to previous node
	Node *aux = reportList;				// Pointer to list

	// search the node and store the previous node
	while(aux != NULL && aux->reportId != id){
		previous = aux;
		aux = aux->next;
	}

	if(aux == NULL) return reportList; 	// if not found, return the original
									// but if found ...

	if(previous == NULL)			// begin of list point to the next
		reportList = aux->next;			// remove on the begin of list
	else							// previous point to the next
		previous->next = aux->next;	// remove on the mid of list

	free(aux); 			// Free memory space (remove the node)
	return reportList;
}

// search the node
Node * search(Node *reportList, int id){
	for(Node *li = reportList; li != NULL; li = li->next)
		if(li->reportId == id)
			return li;

	return NULL;
}

// free the list
void freel(Node *reportList){
	Node *aux = reportList;

	while(aux != NULL){
		Node *next_node = aux->next;
		free(aux);
		aux = next_node;
	}
}

// show each node the list
void showList(Node *reportList){
	for(Node *li = reportList; li != NULL; li = li->next)
		printf("\nREPORT_ID = %d\n" \
               "COMPANY_ID = %d\n"  \
               "TYPE_REPORT = %s\n" \
               "NAME_FILE = %s\n"   \
               "DATE_FORMAT = %s\n",
                li->reportId, li->companyId, li->typeReport, li->nameFile, li->dateFormat);
}
// -----------------------------------------------------------------
// DAT/TAD: Data Abstract Type End
