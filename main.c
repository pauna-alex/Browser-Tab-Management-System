#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_PAGES 50
#define DIE(assertion, call_description)  \
	do                                    \
	{                                     \
		if (assertion)                    \
		{                                 \
			fprintf(stderr, "(%s, %d): ", \
					__FILE__, __LINE__);  \
			perror(call_description);     \
			exit(errno);                  \
		}                                 \
	} while (0)

struct browser
{
	struct tab *current;
	struct tabsList *list;
};

struct page
{
	int id;
	char url[50];
	char *description;
};

struct tab
{
	int id;
	struct page *currentPage;
	struct stack *backwardStack;
	struct stack *forwardStack;
};

typedef struct tabsList_node
{
	struct tab *data;
	struct tabsList_node *prev;
	struct tabsList_node *next;
} tabsList_node;

typedef struct tabsList
{
	tabsList_node *santinel;
	tabsList_node *head;
	unsigned int data_size;
	unsigned int size;
} tabsList;

typedef struct ll_node ll_node;
struct ll_node
{
	struct page *data;
	ll_node *next;
};

typedef struct linked_list linked_list;
struct linked_list
{
	ll_node *head;
	unsigned int data_size;
	unsigned int size;
};

typedef struct stack stack;
struct stack
{
	linked_list *list;
};

linked_list *ll_create(void)
{
	linked_list *ll = (linked_list *)malloc(sizeof(linked_list));
	DIE(!ll, "eroare");

	ll->head = (ll_node *)malloc(sizeof(ll_node));
	DIE(!ll->head, "eroare");

	ll->head->data = NULL;
	ll->head->next = NULL;

	ll->size = 0;
	ll->data_size = sizeof(struct page);

	return ll;
}

ll_node *get_nth_node(linked_list *list, unsigned int n)
{
	if (!list || list->size == 0)
		return NULL;

	if (n >= list->size)
		n = list->size - 1;

	ll_node *curr = list->head->next;
	while (n > 0 && curr)
	{
		curr = curr->next;
		--n;
	}

	return curr;
}

void ll_add_nth_node(linked_list *list, unsigned int n, const void *new_data)
{
	if (!list)
		return;

	if (n > list->size)
		n = list->size;

	ll_node *prev = list->head;
	ll_node *curr = list->head->next;

	while (n > 0)
	{
		prev = curr;
		curr = curr->next;
		--n;
	}

	ll_node *new_node = malloc(sizeof(*new_node));
	DIE(!new_node, "eroare");

	new_node->data = malloc(list->data_size);
	DIE(!new_node->data, "eroare");

	memcpy(new_node->data, new_data, list->data_size);

	new_node->next = curr;
	prev->next = new_node;

	list->size++;
}

ll_node *ll_remove_nth_node(linked_list *list, unsigned int n)
{
	if (!list || list->size == 0)
		return NULL;

	if (n >= list->size)
		n = list->size - 1;

	ll_node *prev = list->head;
	ll_node *curr = list->head->next;

	while (n > 0)
	{
		prev = curr;
		curr = curr->next;
		--n;
	}

	prev->next = curr->next;
	list->size--;

	return curr;
}

unsigned int ll_get_size(linked_list *list)
{
	if (!list)
		return 0;

	return list->size;
}

void ll_free(linked_list **pp_list)
{
	if (!pp_list || !*pp_list)
		return;

	ll_node *curr = (*pp_list)->head->next;
	while (curr)
	{
		ll_node *to_delete = curr;
		curr = curr->next;

		free(to_delete->data->description);
		free(to_delete->data);
		free(to_delete);
	}

	free((*pp_list)->head);
	free(*pp_list);
	*pp_list = NULL;
}

stack *st_create(void)
{
	stack *st = (stack *)malloc(sizeof(stack));
	DIE(!st, "alocare memorie");
	st->list = ll_create();
	return st;
}

unsigned int st_get_size(stack *st)
{
	return ll_get_size(st->list);
}

unsigned int st_is_empty(stack *st)
{
	return st_get_size(st) == 0;
}

void *st_peek(stack *st)
{
	if (st && st->list)
	{
		ll_node *node = get_nth_node(st->list, 0);
		if (node)
			return node->data;
	}
	return NULL;
}

void st_pop(stack *st)
{
	if (st && st->list && st->list->size != 0)
	{
		ll_node *removed = ll_remove_nth_node(st->list, 0);
		free(removed->data->description);
		free(removed->data);
		free(removed);
	}
}

void st_push(stack *st, void *new_data)
{
	if (st && st->list)
		ll_add_nth_node(st->list, 0, new_data);
}

void st_clear(stack *st)
{
	while (st->list->size)
	{
		st_pop(st);
	}
}

void st_free(stack *st)
{
	ll_free(&st->list);
	free(st);
}

tabsList *tabsList_create(void)
{
	tabsList *list = malloc(sizeof(tabsList));
	DIE(!list, "eroare alocare memorie");
	list->santinel = malloc(sizeof(tabsList_node));
	DIE(!list->santinel, "eroare alocare memorie");
	list->santinel->data = NULL;
	list->santinel->next = list->santinel;
	list->santinel->prev = list->santinel;

	list->head = list->santinel;
	list->data_size = sizeof(struct tab);
	list->size = 0;

	return list;
}

tabsList_node *tabsList_get_nth_node(tabsList *list, unsigned int n)
{
	if (!list || list->size == 0)
		return NULL;

	if (n >= list->size)
		n = n % list->size;

	tabsList_node *find = list->head->next;
	for (unsigned int i = 0; i < n; i++)
		find = find->next;

	return find;
}

void tabsList_add_nth_node(tabsList *list, unsigned int n, struct tab *new_data)
{
	if (!list)
		return;

	tabsList_node *new_node = malloc(sizeof(tabsList_node));
	DIE(!new_node, "eroare alocare memorie");
	new_node->data = new_data;

	tabsList_node *curr = list->head;
	for (unsigned int i = 0; i < n && curr->next != list->santinel; i++)
		curr = curr->next;

	new_node->next = curr->next;
	new_node->prev = curr;
	curr->next->prev = new_node;
	curr->next = new_node;

	list->size++;
}

tabsList_node *tabsList_get_node_by_id(tabsList *list, int id)
{
	tabsList_node *node = list->santinel->next;

	while (node != list->santinel)
	{
		if (node->data->id == id)
			return node;
		node = node->next;
	}

	return NULL;
}

tabsList_node *tabsList_remove_nth_node(tabsList *list, unsigned int n)
{
	if (!list || list->size == 0)
		return NULL;

	if (n >= list->size)
		n = n % list->size;

	tabsList_node *curr = list->head;
	for (unsigned int i = 0; i < n && curr->next != list->santinel; i++)
		curr = curr->next;

	tabsList_node *rem = curr->next;
	if (rem == list->santinel)
		return NULL;

	curr->next = rem->next;
	rem->next->prev = curr;

	list->size--;
	return rem;
}

void tabList_free_node(tabsList_node *node)
{
	if (!node)
		return;

	if (node->data)
	{
		struct tab *t = node->data;

		if (t->currentPage)
		{
			free(t->currentPage->description);
			free(t->currentPage);
		}

		st_free(t->backwardStack);
		st_free(t->forwardStack);

		free(t);
	}

	free(node);
}

unsigned int tabsList_get_size(tabsList *list)
{
	if (!list)
		return 0;
	return list->size;
}

void tabsList_free(tabsList **pp_list)
{
	if (!pp_list || !(*pp_list))
		return;

	tabsList_node *curr = (*pp_list)->santinel->next;
	while (curr != (*pp_list)->santinel)
	{
		tabsList_node *to_delete = curr;
		curr = curr->next;

		tabList_free_node(to_delete);
	}

	free((*pp_list)->santinel);
	free(*pp_list);
	*pp_list = NULL;
}

void read_page(FILE *in, struct page **vector)
{
	int page_count;
	fscanf(in, "%d", &page_count);
	*vector = (struct page *)malloc(50 * sizeof(struct page));
	DIE(!(*vector), "eroare");
	memset(*vector, 0, 50 * sizeof(struct page));

	for (int i = 0; i < page_count; i++)
	{
		int poz;
		char url[100];
		char description[100];

		fscanf(in, "%d", &poz);
		fscanf(in, "%s", url);

		while (fgetc(in) != '\n')
			;

		fgets(description, sizeof(description), in);
		size_t len = strlen(description);
		if (len > 0 && description[len - 1] == '\n')
		{
			description[len - 1] = '\0';
		}

		(*vector)[poz - 1].id = poz;
		strcpy((*vector)[poz - 1].url, url);
		(*vector)[poz - 1].description = malloc(strlen(description) + 1);
		DIE(!((*vector)[poz - 1].description), "eroare");
		strcpy((*vector)[poz - 1].description, description);
	}
}

struct browser *create_browser(void)
{
	struct browser *google = (struct browser *)malloc(sizeof(struct browser));
	DIE(!google, "eroare malloc");

	google->list = tabsList_create();
	struct tab *first_tab = (struct tab *)malloc(sizeof(struct tab));
	DIE(!first_tab, "eroare malloc");
	first_tab->currentPage = (struct page *)malloc(sizeof(struct page));
	DIE(!first_tab->currentPage, "eroare maloc");
	memset(first_tab->currentPage, 0, sizeof(struct page));

	first_tab->currentPage->id = 0;
	first_tab->currentPage->description =
										malloc(strlen("Computer Science") + 1);
	DIE(!first_tab->currentPage->description, "eroare malloc");

	strcpy(first_tab->currentPage->description, "Computer Science");
	strcpy(first_tab->currentPage->url, "https://acs.pub.ro/");
	first_tab->id = 0;
	first_tab->backwardStack = st_create();
	first_tab->forwardStack = st_create();

	tabsList_add_nth_node(google->list, 0, first_tab);
	google->current = tabsList_get_nth_node(google->list, 0)->data;

	return google;
}

void new_tab(struct browser *google, int nr)
{
	struct tab *new_tab = (struct tab *)malloc(sizeof(struct tab));
	DIE(!new_tab, "eroare malloc");
	struct page *first_page = (struct page *)malloc(sizeof(struct page));
	DIE(!first_page, "eroare malloc");
	memset(first_page, 0, sizeof(struct page));

	first_page->id = 0;
	strcpy(first_page->url, "https://acs.pub.ro/");
	first_page->description = malloc(strlen("Computer Science") + 1);
	DIE(!first_page->description, "eroare malloc");
	strcpy(first_page->description, "Computer Science");

	new_tab->currentPage = first_page;
	new_tab->id = nr;
	new_tab->backwardStack = st_create();
	new_tab->forwardStack = st_create();
	tabsList_add_nth_node(google->list, google->list->size, new_tab);
	google->current =
			tabsList_get_nth_node(google->list, google->list->size - 1)->data;
}

void next(struct browser *google, FILE *out)
{
	if (tabsList_get_size(google->list) < 1)
	{
		fprintf(out, "403 Forbidden\n");
		return;
	}

	struct tab *to = google->current;
	tabsList_node *node = google->list->santinel->next;

	while (node != google->list->santinel)
	{
		if (node->data == to)
		{
			break;
		}
		node = node->next;
	}

	tabsList_node *next_node = node->next;
	if (next_node == google->list->santinel)
	{
		next_node = google->list->santinel->next;
	}

	google->current = next_node->data;
}

void print(struct browser *google, FILE *out)
{
	struct tab *to = google->current;
	tabsList_node *node = google->list->santinel->next;

	while (node != google->list->santinel)
	{
		if (node->data == to)
		{
			break;
		}
		node = node->next;
	}
	fprintf(out, "%d", node->data->id);
	node = node->next;
	if (node == google->list->santinel)
		node = google->list->santinel->next;

	while (node->data != to)
	{
		fprintf(out, " %d", node->data->id);
		node = node->next;
		if (node == google->list->santinel)
			node = google->list->santinel->next;
	}

	fprintf(out, "\n");
	fprintf(out, "%s\n", to->currentPage->description);
}

void open(struct browser *google, FILE *out, FILE *in)
{
	int nr;
	fscanf(in, "%d", &nr);

	tabsList_node *node = tabsList_get_node_by_id(google->list, nr);
	if (!node)
		fprintf(out, "403 Forbidden\n");
	else
		google->current = node->data;
}

void prev(struct browser *google, FILE *out)
{
	if (tabsList_get_size(google->list) < 1)
	{
		fprintf(out, "403 Forbidden\n");
		return;
	}

	struct tab *aj = google->current;
	tabsList_node *node = google->list->santinel->next;

	while (node != google->list->santinel)
	{
		if (node->data == aj)
		{
			break;
		}
		node = node->next;
	}

	tabsList_node *prev_node = node->prev;
	if (prev_node == google->list->santinel)
	{
		prev_node = google->list->santinel->prev;
	}

	google->current = prev_node->data;
}

void close(struct browser *google, FILE *out)
{
	if (google->current->id == 0)
	{
		fprintf(out, "403 Forbidden\n");
		return;
	}

	struct tab *to = google->current;
	tabsList_node *node = google->list->santinel->next;
	int nr = 0;

	while (node != google->list->santinel)
	{
		if (node->data == to)
			break;
		node = node->next;
		nr++;
	}

	tabsList_node *rem = tabsList_remove_nth_node(google->list, nr);

	if (rem->prev == google->list->santinel)
		google->current = google->list->santinel->prev->data;
	else
		google->current = rem->prev->data;

	tabList_free_node(rem);
}

void add_page(struct browser *google, struct page *vector, FILE *out, FILE *in)
{
	int nr;
	fscanf(in, "%d", &nr);

	if (nr <= 0 || vector[nr - 1].id == 0) {
		fprintf(out, "403 Forbidden\n");

	}	else {
		struct tab *tab_cur = google->current;

		struct page *new_page = (struct page *)malloc(sizeof(struct page));
		DIE(!new_page, "eroare malloc");
		memset(new_page, 0, sizeof(struct page));

		struct page *src = &vector[nr - 1];
		// printf("aici:%d %s %s\n", src->id, src->url, src->description);
		new_page->id = src->id;
		strcpy(new_page->url, src->url);
		new_page->description = malloc(strlen(src->description) + 1);
		DIE(!new_page->description, "eroare");

		strcpy(new_page->description, src->description);

		if (tab_cur->currentPage)
		{
			struct page *back_copy = (struct page *)malloc(sizeof(struct page));
			DIE(!back_copy, "eroare");
			memset(back_copy, 0, sizeof(struct page));

			back_copy->id = tab_cur->currentPage->id;
			strcpy(back_copy->url, tab_cur->currentPage->url);
			back_copy->description =
						malloc(strlen(tab_cur->currentPage->description) + 1);
			DIE(!back_copy->description, "eroare");
			strcpy(back_copy->description, tab_cur->currentPage->description);

			st_push(tab_cur->backwardStack, back_copy);
			st_clear(tab_cur->forwardStack);

			free(back_copy);
			free(tab_cur->currentPage->description);
			free(tab_cur->currentPage);
		}

		tab_cur->currentPage = new_page;
	}
}

void forward(struct browser *google, FILE *out)
{
	struct tab *tab_cur = google->current;

	if (!st_is_empty(tab_cur->forwardStack)) {
		struct page *page_cur = st_peek(tab_cur->forwardStack);

		struct page *new_page = (struct page *)malloc(sizeof(struct page));
		DIE(!new_page, "eroare malloc");
		memset(new_page, 0, sizeof(struct page));

		new_page->id = page_cur->id;
		strcpy(new_page->url, page_cur->url);
		new_page->description = malloc(strlen(page_cur->description) + 1);
		DIE(!new_page->description, "eroare malloc");

		strcpy(new_page->description, page_cur->description);

		st_pop(tab_cur->forwardStack);

		struct page *copy = (struct page *)malloc(sizeof(struct page));
		DIE(!copy, "eroare malloc");
		memset(copy, 0, sizeof(struct page));

		copy->id = tab_cur->currentPage->id;
		strcpy(copy->url, tab_cur->currentPage->url);
		copy->description =
						malloc(strlen(tab_cur->currentPage->description) + 1);
		DIE(!copy->description, "eroare malloc");

		strcpy(copy->description, tab_cur->currentPage->description);

		st_push(tab_cur->backwardStack, copy);
		free(copy);

		if (tab_cur->currentPage)
		{
			free(tab_cur->currentPage->description);
			free(tab_cur->currentPage);
		}

		tab_cur->currentPage = new_page;
	}	else {
		fprintf(out, "403 Forbidden\n");
	}
}

void backward(struct browser *google, FILE *out)
{
	struct tab *tab_cur = google->current;

	if (!st_is_empty(tab_cur->backwardStack)) {
		struct page *page_cur = st_peek(tab_cur->backwardStack);

		struct page *new_page = (struct page *)malloc(sizeof(struct page));
		DIE(!new_page, "eroarea malloc");
		memset(new_page, 0, sizeof(struct page));

		new_page->id = page_cur->id;
		strcpy(new_page->url, page_cur->url);
		new_page->description = malloc(strlen(page_cur->description) + 1);
		DIE(!new_page->description, "eroare");

		strcpy(new_page->description, page_cur->description);
		st_pop(tab_cur->backwardStack);

		struct page *copy = (struct page *)malloc(sizeof(struct page));
		DIE(!copy, "rroare");
		memset(copy, 0, sizeof(struct page));

		copy->id = tab_cur->currentPage->id;
		strcpy(copy->url, tab_cur->currentPage->url);
		copy->description =
						malloc(strlen(tab_cur->currentPage->description) + 1);

		strcpy(copy->description, tab_cur->currentPage->description);

		st_push(tab_cur->forwardStack, copy);

		free(copy);

		if (tab_cur->currentPage)
		{
			free(tab_cur->currentPage->description);
			free(tab_cur->currentPage);
		}

		tab_cur->currentPage = new_page;
	}	else {
		fprintf(out, "403 Forbidden\n");
	}
}

void printh(struct browser *google, FILE *out, FILE *in)
{
	int tab_id;
	fscanf(in, "%d", &tab_id);

	tabsList_node *node = tabsList_get_node_by_id(google->list, tab_id);
	if (!node)
	{
		fprintf(out, "403 Forbidden\n");
		return;
	}

	struct tab *tab = node->data;
	struct page *forward_urls[MAX_PAGES];
	int forward_count = 0;

	ll_node *curr = tab->forwardStack->list->head->next;
	while (curr)
	{
		forward_urls[forward_count] = (struct page *)curr->data;
		forward_count++;
		curr = curr->next;
	}

	for (int i = forward_count - 1; i >= 0; i--)
	{
		fprintf(out, "%s\n", forward_urls[i]->url);
	}

	if (tab->currentPage)
	{
		fprintf(out, "%s\n", tab->currentPage->url);
	}

	curr = tab->backwardStack->list->head->next;
	while (curr)
	{
		struct page *page = (struct page *)curr->data;
		fprintf(out, "%s\n", page->url);
		curr = curr->next;
	}
}

void read_commands(FILE *in, FILE *out, struct browser *b, struct page *vector)
{
	int nr_commands;
	fscanf(in, "%d", &nr_commands);
	int index = 1;
	for (int i = 0; i < nr_commands; i++)
	{
		char command[256];
		fscanf(in, "%s", command);

		if (strcmp(command, "NEW_TAB") == 0) {
			new_tab(b, index);
			index++;
		} else if (strcmp(command, "OPEN") == 0) {
			open(b, out, in);
		} else if (strcmp(command, "NEXT") == 0) {
			next(b, out);
		} else if (strcmp(command, "PREV") == 0) {
			prev(b, out);
		} else if (strcmp(command, "PAGE") == 0) {
			add_page(b, vector, out, in);
		} else if (strcmp(command, "BACKWARD") == 0) {
			backward(b, out);
		} else if (strcmp(command, "FORWARD") == 0) {
			forward(b, out);
		} else if (strcmp(command, "CLOSE") == 0) {
			close(b, out);
		} else if (strcmp(command, "PRINT") == 0) {
			print(b, out);
		} else if (strcmp(command, "PRINT_HISTORY") == 0) {
			printh(b, out, in);
		}
	}
}

int main(void)
{
	FILE *in = fopen("tema1.in", "r");
	DIE(!in, "eroare fopen");
	FILE *out = fopen("tema1.out", "w");
	DIE(!out, "eroare fopen");

	struct page *vector = NULL;

	read_page(in, &vector);
	struct browser *google = create_browser();

	read_commands(in, out, google, vector);

	for (int i = 0; i < 50; i++)
	{
		free(vector[i].description);
	}
	free(vector);

	tabsList_free(&google->list);
	free(google);

	fclose(in);
	fclose(out);

	return 0;
}
