//#include <stdio.h>
//#include <malloc.h>
//#include "stack.h"
//#define _CRT_SECURE_NO_WARNINGS
//void InitStack(StackL* s)
//{
//	s->top = NULL;
//}
//int Push(StackL* s, enemy v)
//{
//	struct LE* p = (struct LE*)malloc(sizeof(struct LE));
//		if (!p) return 0;
//	p->next = s->top;
//	p->v = v;
//	s->top = p;
//	return 1;
//}
//enemy Pop(StackL* s)
//{
//	struct LE* p;
//
//	if (s->top)
//	{
//		p = s->top;
//		s->top = s->top->next;
//
//		free(p);
//	}
//
//}
//enemy Peek(StackL const* s)
//{
//	if (s->top)
//		return s->top->v;
//}
//int IsEmptyStack(StackL const* s)
//{
//	return s->top == NULL;
//}
//void Clear(StackL* s)
//{
//	while (!IsEmptyStack(s)) Pop(s);
//}
//
//
//void Destruct(StackL * s)
//{
//	Clear(s);
//}