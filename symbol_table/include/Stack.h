typedef struct Stack{
	void **base;
	void **top;		/// 
	int stacksize;	/// size
}Stack;
void stack_init(Stack *stack,int initsize);
void *stack_push(Stack *stack,void *element,int size);
void stack_pop(Stack *stack);
void *stack_get_top(Stack *stack);
int stack_is_empty(Stack *stack);
void stack_destory(Stack *stack);