// Biblioteca de Estruturas em �rvore bin�ria
// Fun��es de inser��o, remo��o e busca bin�ria
//
// Author: Wenderson
// Data  : 31/10/2023
//

// Estrutura da �rvore -------------------------------------------------
typedef struct tree {
	int id;					// informa��o de inteiro
	int index;
	struct tree * sae;		// Ponteiro de Sub�rvore Esquerda
	struct tree * sad; 		// Ponteiro de Sub�rvore Direita
}Tree;
// ---------------------------------------------------------------------

// Fun��es de Opera��o da �rvore
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Inicializa��o da �rvore ---------------------------------------------
Tree * initialize(){
	return NULL;			// retorne NULO para o ponteiro
}
// ---------------------------------------------------------------------


// Verifica��o de �rvore Vazia -----------------------------------------
bool empty(Tree *tree){
	return (tree == NULL);	// Se � NULO, ent�o est� vazia!
}
// ---------------------------------------------------------------------


// Cria��o de um n�-raiz -----------------------------------------------
// Entrada: integer -> informa��o
//			Tree* 	-> Sub�rvore Esquerda
//			Tree* 	-> Sub�rvore Direita
// Sa�da: 	Tree* 	-> Endere�o do n�-raiz alocado
Tree * create(int id, int index, Tree *sae, Tree *sad){
	Tree *tree = (Tree*) malloc(sizeof(Tree));	// Aloca n� na mem�ria
	tree->id = id;								// Atribue a informa��o
	tree->index = index;
	tree->sae = sae;							// Atribue o SAE
	tree->sad = sad;							// Atribue o SAD
	return tree;								// Retorne o endere�o do n�
}
// ---------------------------------------------------------------------



// Impress�o em Pr�-ordem ----------------------------------------------
// NED -> N�, Esquerda, Direita = NLR (Node, Left, Right)
// Entrada: Tree*  -> N�-raiz/Sub�rvore
int i = -1;
void showNLR(Tree *tree){
	i++;
	if(!empty(tree)){
		for(int j = 0; j < i; j++)	printf("   |");
		printf("___%d\n", tree->id);
		showNLR(tree->sae);
		showNLR(tree->sad);
	}
	i--;
}
// ---------------------------------------------------------------------

// Impress�o em ordem assim�trica --------------------------------------
// END -> Esquerda, N�, Direita = LNR (Left, Node, Right)
// Entrada: Tree*  -> N�-raiz/Sub�rvore
void showLNR(Tree *tree){
	if(!empty(tree)){
		showLNR(tree->sae);
		printf("%d ", tree->id);
		showLNR(tree->sad);
	}
}
// ---------------------------------------------------------------------

// Impress�o em P�s-ordem ----------------------------------------------
// EDN -> Esquerda, Direita, N� = LRN (Left, Right, Node)
// Entrada: Tree*  -> N�-raiz/Sub�rvore
void showLRN(Tree *tree){
	if(!empty(tree)){
		showLRN(tree->sae);
		showLRN(tree->sad);
		printf("%d ", tree->id);
	}
}
// ---------------------------------------------------------------------

// Inser��o Recursiva (implementa��o pr�pria) --------------------------
// Entrada: Tree*  	-> N�-raiz/Sub�rvore
// Entrada: integer -> Informa��o pra adicionar
// Sa�da  : void/nenhum
void insert(Tree *t1, int id, int index){
	if(id > t1->id)
		if(empty(t1->sad))
			t1->sad = create(id, index, initialize(), initialize());
		else
			insert(t1->sad, id, index);
	else
		if(empty(t1->sae))
			t1->sae = create(id, index, initialize(), initialize());
		else
			insert(t1->sae, id, index);
}
// ---------------------------------------------------------------------

// Busca Recursiva (implementa��o pr�pria) -----------------------------
// Entrada: Tree*  	-> N�-raiz/Sub�rvore
// Entrada: integer -> Informa��o pra buscar
// Sa�da  : Tree*   -> Endere�o encontrado (NULO se falha)
Tree * find(Tree *t1, int id){
	if(empty(t1) || (t1->sad == t1->sae && t1->id != id))
		return NULL;
	else if(t1->id == id)
			return t1;
		 else
			return (id > t1->id) ? find(t1->sad, id) : find(t1->sae, id);
}
// ---------------------------------------------------------------------

bool del_root_wsae(Tree *t1, int id){

	Tree * aux = t1;
	Tree * aux2 = t1;

	for(t1 = t1->sae; !empty(t1->sad); t1 = t1->sad)
		aux2 = t1;

	if(aux2->id != id)
		aux2->sad = (empty(t1->sae)) ? NULL : t1->sae;
	else
		aux2->sae = (empty(t1->sae)) ? NULL : t1->sae;

	aux->id = t1->id;
	free(t1);
	t1 = aux;
	return true;
}

bool del_root_wsad(Tree *t1, int id){

	Tree * aux = t1;
	Tree * aux2 = t1;

	for(t1 = t1->sad; !empty(t1->sae); t1 = t1->sae)
		aux2 = t1;

	if(aux2->id != id)
		aux2->sae = (empty(t1->sad)) ? NULL : t1->sad;
	else
		aux2->sad = (empty(t1->sad)) ? NULL : t1->sad;

	aux->id = t1->id;
	free(t1);
	t1 = aux;
	return true;
}

bool del_node(bool right, Tree *aux, Tree *dir){
	if(right){
		free(aux->sad);
		aux->sad = dir;
	}else{
		free(aux->sae);
		aux->sae = dir;
	}
	return true;
}

// Remo��o de n�s (implementa��o pr�pria) ------------------------------
// Entrada: Tree*  	-> N�-raiz/Sub�rvore
// Entrada: integer -> Informa��o pra remover
// Sa�da  : bool 	-> Se sucesso/falha
//
// Descri��o: Este m�todo de remo��o exclui o n� mais a direta da SAE
// de forma padr�o, tendo ou n�o a SAD. A partir do momento que falta
// a SAE, ent�o o m�todo exclui o n� mais a esquerda da SAD.
// Para remo��o de folha, a 1� condicional � executada.
// Para remo��o de n� com 1 filho SAD, a 2� condicional � executada.
// Para remo��o de n� com 1 filho SAE, a 3� condicional � executada.
// Para remo��o de n� com 2 filhos (SAD e SAE), a 4� � executada.
// O c�digo da 2� e 3� condicional cont�m o algoritmo da 4� condicional +
// um controle adicional. No entanto, a 2� condicional troca-se os ponteiros.

bool remove(Tree *t1, int id){
	Tree *aux, *aux2;
	bool right = false;
	bool root = true;

	aux = t1;
	while(t1->id != id && (!empty(t1->sad) || !empty(t1->sae))){
		root = false;
		aux = t1;
		right = (id > t1->id);
		t1 = (id > t1->id) ? t1->sad : t1->sae;
	}

	if(t1->id != id)
		return false;


	if(empty(t1->sad) && empty(t1->sae)){
		// Remo��o de n� folha
		if(!root)					// Se n�o � a raiz
			return del_node(right, aux, NULL);

		free(t1);
	}else
		if(!empty(t1->sad) && empty(t1->sae)){
			// Remo��o de n� com 1 filho SAD

			return (root) ? del_root_wsad(t1, id) : del_node(right, aux, t1->sad);
		}else if((empty(t1->sad) && !empty(t1->sae))){
		 	// Remo��o de n� com 1 filho SAE

			return (root) ? del_root_wsae(t1, id) : del_node(right, aux, t1->sae);
		}else{
			// Remo��o de n� com 2 filhos: N� mais a direita da SAE

			return del_root_wsae(t1, id);
		}

		return false;
}
// ---------------------------------------------------------------------

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
