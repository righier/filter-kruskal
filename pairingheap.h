#pragma once

#include "utils.h"

struct PairingHeap {

	pair<float,int> e;
	PairingHeap *child = NULL;
	PairingHeap *next = NULL;
	PairingHeap *prev = NULL;

	PairingHeap(float val, int data) {
		e.first = val;
		e.second = data;
	}

};

pair<float,int> top(PairingHeap *h) {
	assert(h != NULL);
	return h->e;
}

PairingHeap *merge(PairingHeap *h1, PairingHeap *h2) {
	if (h1 == NULL) return h2;
	if (h2 == NULL) return h1;
	if (h1->e > h2->e) swap(h1, h2);
	if (h1->child != NULL) h1->child->prev = h2;
	h2->next = h1->child;
	h2->prev = h1;
	h1->child = h2;
	h1->next = NULL;
	h1->prev = NULL;
	return h1;
}

PairingHeap *mergePairs(PairingHeap *h) {
	if (h == NULL) return NULL;
	if (h->next == NULL) return h;
	return merge(merge(h, h->next), mergePairs(h->next->next));
}

PairingHeap *pop(PairingHeap *h) {
	assert(h != NULL);
	PairingHeap *out = mergePairs(h->child);
	if (out != NULL) out->prev = NULL;
	return out;
}

PairingHeap *decreaseKey(PairingHeap *h, PairingHeap *p, float val) {
	assert(p != NULL);
	if (p->e.first <= val) return h;

	p->e.first = val;
	if (h == p) return h;
	if (h == NULL) return p;

	if (p->prev != NULL) {
		if (p == p->prev->next) p->prev->next = p->next;
		else p->prev->child = p->next;
	}
	if (p->next != NULL) p->next->prev = p->prev;
	p->prev = NULL;
	p->next = NULL;

	return merge(h, p);
}