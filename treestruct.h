// Biblioteca de Estruturas em Árvore binária
// Funções de inserção, remoção e busca binária
//
// Author: Wenderson
// Data  : 31/10/2023
//

// Estrutura da Árvore -------------------------------------------------
typedef struct tree {
	int id;					// informação de inteiro
	int index;
	struct tree * sae;		// Ponteiro de SubÁrvore Esquerda
	struct tree * sad; 		// Ponteiro de SubÁrvore Direita
}Tree;
// ---------------------------------------------------------------------

// Funções de Operação da Árvore
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Inicialização da Árvore ---------------------------------------------
Tree * initialize(){
	return NULL;			// retorne NULO para o ponteiro
}
// ---------------------------------------------------------------------


// Verificação de Árvore Vazia -----------------------------------------
bool empty(Tree *tree){
	return (tree == NULL);	// Se é NULO, então está vazia!
}
// ---------------------------------------------------------------------


// Criação de um nó-raiz -----------------------------------------------
// Entrada: integer -> informação
//			Tree* 	-> SubÁrvore Esquerda
//			Tree* 	-> SubÁrvore Direita
// Saída: 	Tree* 	-> Endereço do nó-raiz alocado
Tree * create(int id, int index, Tree *sae, Tree *sad){
	Tree *tree = (Tree*) malloc(sizeof(Tree));	// Aloca nó na memória
	tree->id = id;								// Atribue a informação
	tree->index = index;
	tree->sae = sae;							// Atribue o SAE
	tree->sad = sad;							// Atribue o SAD
	return tree;								// Retorne o endereço do nó
}
// ---------------------------------------------------------------------



// Impressão em Pré-ordem ----------------------------------------------
// NED -> Nó, Esquerda, Direita = NLR (Node, Left, Right)
// Entrada: Tree*  -> Nó-raiz/SubÁrvore
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

// Impressão em ordem assimétrica --------------------------------------
// END -> Esquerda, Nó, Direita = LNR (Left, Node, Right)
// Entrada: Tree*  -> Nó-raiz/SubÁrvore
void showLNR(Tree *tree){
	if(!empty(tree)){
		showLNR(tree->sae);
		printf("%d ", tree->id);
		showLNR(tree->sad);
	}
}
// ---------------------------------------------------------------------

// Impressão em Pós-ordem ----------------------------------------------
// EDN -> Esquerda, Direita, Nó = LRN (Left, Right, Node)
// Entrada: Tree*  -> Nó-raiz/SubÁrvore
void showLRN(Tree *tree){
	if(!empty(tree)){
		showLRN(tree->sae);
		showLRN(tree->sad);
		printf("%d ", tree->id);
	}
}
// ---------------------------------------------------------------------

// Inserção Recursiva (implementação própria) --------------------------
// Entrada: Tree*  	-> Nó-raiz/SubÁrvore
// Entrada: integer -> Informação pra adicionar
// Saída  : void/nenhum
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

// Busca Recursiva (implementação própria) -----------------------------
// Entrada: Tree*  	-> Nó-raiz/SubÁrvore
// Entrada: integer -> Informação pra buscar
// Saída  : Tree*   -> Endereço encontrado (NULO se falha)
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

// Remoção de nós (implementação própria) ------------------------------
// Entrada: Tree*  	-> Nó-raiz/SubÁrvore
// Entrada: integer -> Informação pra remover
// Saída  : bool 	-> Se sucesso/falha
//
// Descrição: Este método de remoção exclui o nó mais a direta da SAE
// de forma padrão, tendo ou não a SAD. A partir do momento que falta
// a SAE, então o método exclui o nó mais a esquerda da SAD.
// Para remoção de folha, a 1ª condicional é executada.
// Para remoção de nó com 1 filho SAD, a 2ª condicional é executada.
// Para remoção de nó com 1 filho SAE, a 3ª condicional é executada.
// Para remoção de nó com 2 filhos (SAD e SAE), a 4ª é executada.
// O código da 2ª e 3ª condicional contém o algoritmo da 4ª condicional +
// um controle adicional. No entanto, a 2ª condicional troca-se os ponteiros.

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
		// Remoção de nó folha
		if(!root)					// Se não é a raiz
			return del_node(right, aux, NULL);

		free(t1);
	}else
		if(!empty(t1->sad) && empty(t1->sae)){
			// Remoção de nó com 1 filho SAD

			return (root) ? del_root_wsad(t1, id) : del_node(right, aux, t1->sad);
		}else if((empty(t1->sad) && !empty(t1->sae))){
		 	// Remoção de nó com 1 filho SAE

			return (root) ? del_root_wsae(t1, id) : del_node(right, aux, t1->sae);
		}else{
			// Remoção de nó com 2 filhos: Nó mais a direita da SAE

			return del_root_wsae(t1, id);
		}

		return false;
}
// ---------------------------------------------------------------------

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
