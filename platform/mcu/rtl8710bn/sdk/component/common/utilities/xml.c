/*
Author: Alex Fang

Tree Structure
              root              root
              | ^                ^
          child |                |
              | parent         parent
              v |                |
NULL<-prev-child1-next-><-prev-child2-next->NULL
*/

#include "platform/platform_stdlib.h"
#include "FreeRTOS.h"
#include "xml.h"

static struct xml_node *_xml_new_element(char *prefix, char *name, char *uri, char *attr);

char *xml_strstr(const char *str1, const char *str2) {
	char *a, *b;

	/* First scan quickly through the two strings looking for a
	* single-character match.  When it's found, then compare the
	* rest of the substring.
	*/

	b = (char *)str2;
	if (*b == 0) {
		return (char *)str1;
	}
	for ( ; *str1 != 0; str1 += 1) {
		if (*str1 != *b) {
			continue;
		}
		a = (char *)str1;
		while (1) {
			if (*b == 0) {
				return (char *)str1;
			}
			if (*a++ != *b++) {
				break;
			}
		}
		b = (char *)str2;
	}
	return (char *) 0;
}

static void *xml_malloc(unsigned int size)
{
	return pvPortMalloc(size);
}

void xml_free(void *buf)
{
	vPortFree(buf);
}

static char *str_strip(char *str, unsigned int str_len)
{ 
	char *front, *rear;
	char *strip = NULL;
	int strip_len;

	if(!str || (str_len <= 0))
		return NULL;

	for(front = str; front < (str + str_len); front ++)
		if(*front != ' ') break;
	
	if(front == (str + str_len))
		return NULL;
	
	for(rear = (str + str_len - 1); rear >= front; rear --)
		if(*rear != ' ') break;

	if(front == rear) {
		strip_len = 1;
		strip = (char *) xml_malloc(strip_len + 1);
		memcpy(strip, front, strip_len);
		strip[strip_len] = '\0';
	}
	else {
		strip_len = rear + 1 - front;
		strip = (char *) xml_malloc(strip_len + 1);
		memcpy(strip, front, strip_len);
		strip[strip_len] = '\0';
	}

	return strip;
}

/* TAG Format:
 *    STag ::= '<' Name (S Attribute)* S? '>'
 *    ETag ::= '</' Name S? '>'
 *    EmptyElemTag ::= '<' Name (S Attribute)* S? '/>'
 */
static void _parse_tag(char *tag, char **prefix, char **name, char **uri, char **attr)
{
	char *prefix_char, *ns_tag, *ns_front;
	int have_prefix = 0;
	int have_uri = 0;

	prefix_char = strchr(tag, ':');

	if(prefix_char) {
		char *tag_sep = strchr(tag, ' ');

		if(!tag_sep)
			have_prefix = 1;
		else if(prefix_char < tag_sep)
			have_prefix = 1;
	}

	if(have_prefix) {
		*prefix = str_strip(tag, prefix_char - tag);
		ns_tag = (char *) xml_malloc(strlen(" xmlns:") + strlen(*prefix) + 1);
		sprintf(ns_tag, " xmlns:%s", *prefix);
		ns_front = xml_strstr(tag, ns_tag);
		xml_free(ns_tag);
	}
	else {
		*prefix = NULL;
		ns_tag = " xmlns";
		ns_front = xml_strstr(tag, ns_tag);
	}

	if(ns_front)
		have_uri = 1;

	if(have_prefix && have_uri) {
		char *uri_front, *uri_rear, *tag_sep, ns_sep;
		int uri_len;

		tag_sep = strchr(prefix_char + 1, ' ');
		*name = str_strip(prefix_char + 1, tag_sep - (prefix_char + 1));

		if(attr)
			*attr = str_strip(tag_sep, tag + strlen(tag) - tag_sep);

		if(*(strchr(ns_front, '=') + 1) == '\'')
			ns_sep = '\'';
		else
			ns_sep = '\"';

		uri_front = strchr(ns_front, ns_sep) + 1;
		uri_rear = strchr(uri_front, ns_sep);
		uri_len = uri_rear - uri_front;
		*uri = (char *) xml_malloc(uri_len + 1);
		memcpy(*uri, uri_front, uri_len);
		(*uri)[uri_len] = '\0';
	}
	else if(have_prefix) {
		char *tag_sep;

		*uri = NULL;
		tag_sep = strchr(prefix_char + 1, ' ');

		if(tag_sep) {
			*name = str_strip(prefix_char + 1, tag_sep - (prefix_char + 1));

			if(attr)
				*attr = str_strip(tag_sep, tag + strlen(tag) - tag_sep);
		}
		else {
			*name = str_strip(prefix_char + 1, tag + strlen(tag) - (prefix_char + 1));

			if(attr)
				*attr = NULL;
		}
	}
	else if(have_uri) {
		char *uri_front, *uri_rear, *tag_sep, ns_sep;
		int uri_len;

		tag_sep = strchr(tag, ' ');
		*name = str_strip(tag, tag_sep - tag);

		if(attr)
			*attr = str_strip(tag_sep, tag + strlen(tag) - tag_sep);

		if(*(strchr(ns_front, '=') + 1) == '\'')
			ns_sep = '\'';
		else
			ns_sep = '\"';

		uri_front = strchr(ns_front, ns_sep) + 1;
		uri_rear = strchr(uri_front, ns_sep);
		uri_len = uri_rear - uri_front;
		*uri = (char *) xml_malloc(uri_len + 1);
		memcpy(*uri, uri_front, uri_len);
		(*uri)[uri_len] = '\0';
	}
	else {
		char *tag_sep;

		*uri = NULL;
		tag_sep = strchr(tag, ' ');

		if(tag_sep) {
			*name = str_strip(tag, tag_sep - tag);

			if(attr)
				*attr = str_strip(tag_sep, tag + strlen(tag) - tag_sep);
		}
		else {
			*name = str_strip(tag, strlen(tag));

			if(attr)
				*attr = NULL;
		}
	}
}

static void parse_tag(char *tag, char **prefix, char **name, char **uri)
{
	_parse_tag(tag, prefix, name, uri, NULL);
}

int xml_doc_name(char *doc_buf, int doc_len, char **doc_prefix, char **doc_name, char **doc_uri)
{
	char *xml_buf, *cur_pos, *tag_front, *tag_rear;
	char *start_tag, *end_tag1, *end_tag2;
	int tag_len, ret = -1;

	xml_buf = (char *) xml_malloc(doc_len + 1);
	memcpy(xml_buf, doc_buf, doc_len);
	xml_buf[doc_len] = '\0';

	cur_pos = xml_buf;

	while(cur_pos < (xml_buf + doc_len)) {
		if((tag_front = strchr(cur_pos, '<')) != NULL) {
			tag_front ++;

			if((tag_rear = strchr(tag_front, '>')) != NULL) {
				char *prefix = NULL, *name = NULL, *uri = NULL;

				//Element without content
				if(*(tag_rear - 1) == '/') {
					tag_len = tag_rear - 1 - tag_front;
					start_tag = (char *) xml_malloc(tag_len + 1);
					memcpy(start_tag, tag_front, tag_len);
					start_tag[tag_len] = '\0';
					parse_tag(start_tag, &prefix, &name, &uri);
					xml_free(start_tag);
					*doc_name = name;
					*doc_prefix = prefix;
					*doc_uri = uri;
					ret = 0;
					cur_pos = xml_buf + doc_len;
				}
				//Element with content
				else {
					tag_len = tag_rear - tag_front;
					start_tag = (char *) xml_malloc(tag_len + 1);
					memcpy(start_tag, tag_front, tag_len);
					start_tag[tag_len] = '\0';
					parse_tag(start_tag, &prefix, &name, &uri);
					xml_free(start_tag);

					if(prefix) {
						end_tag1 = (char *) xml_malloc(strlen(prefix) + strlen(name) + 5);
						sprintf(end_tag1, "</%s:%s>", prefix, name);
						end_tag2 = (char *) xml_malloc(strlen(prefix) + strlen(name) + 5);
						sprintf(end_tag2, "</%s:%s ", prefix, name);
					}
					else {
						end_tag1 = (char *) xml_malloc(strlen(name) + 4);
						sprintf(end_tag1, "</%s>", name);
						end_tag2 = (char *) xml_malloc(strlen(name) + 4);
						sprintf(end_tag2, "</%s ", name);
					}

					if(xml_strstr(tag_rear + 1, end_tag1) || xml_strstr(tag_rear + 1, end_tag2)) {
						*doc_name = name;
						*doc_prefix = prefix;
						*doc_uri = uri;
						ret = 0;
						cur_pos = xml_buf + doc_len;
					}
					else {
						xml_free(name);
						if(prefix) xml_free(prefix);
						if(uri)	xml_free(uri);
						cur_pos = tag_rear + 1;
					}

					xml_free(end_tag1);
					xml_free(end_tag2);
				}
			}
			else {
				cur_pos = xml_buf + doc_len;
			}
		}
		else {
			cur_pos = xml_buf + doc_len;
		}
	}

	xml_free(xml_buf);
	
	return ret;
}

struct xml_node *_xml_parse_doc(char *doc_buf, int doc_len, struct xml_node *root)
{
	char *xml_buf, *cur_pos;

	xml_buf = (char *) xml_malloc(doc_len + 1);
	memcpy(xml_buf, doc_buf, doc_len);
	xml_buf[doc_len] = '\0';

	cur_pos = xml_buf;

	while(cur_pos < (xml_buf + doc_len)) {
		char *tag_front, *tag_rear;
		struct xml_node *node;
		
		if((tag_front = strchr(cur_pos, '<')) != NULL) {
			tag_front ++;

			if((tag_rear = strchr(tag_front, '>')) != NULL) {
				char *doc_front, *doc_rear, *start_tag, *end_tag1, *end_tag2;
				char *prefix = NULL, *name = NULL, *uri = NULL, *attr = NULL;
				int tag_len;

				//Element without content
				if(*(tag_rear - 1) == '/') {
					doc_front = tag_rear + 1;
					tag_len = tag_rear - 1 - tag_front;
					start_tag = (char *) xml_malloc(tag_len + 1);
					memcpy(start_tag, tag_front, tag_len);
					start_tag[tag_len] = '\0';
					_parse_tag(start_tag, &prefix, &name, &uri, &attr);
					node = _xml_new_element(prefix, name, uri, attr);

					if(root) {
						xml_add_child(root, node);
						cur_pos = doc_front;
					}
					else {
						root = node;
						cur_pos = xml_buf + doc_len;
					}
				}
				//Element with content
				else {
					doc_front = tag_rear + 1;
					tag_len = tag_rear - tag_front;
					start_tag = (char *) xml_malloc(tag_len + 1);
					memcpy(start_tag, tag_front, tag_len);
					start_tag[tag_len] = '\0';
					_parse_tag(start_tag, &prefix, &name, &uri, &attr);

					if(prefix) {
						end_tag1 = (char *) xml_malloc(strlen(prefix) + strlen(name) + 5);
						sprintf(end_tag1, "</%s:%s>", prefix, name);
						end_tag2 = (char *) xml_malloc(strlen(prefix) + strlen(name) + 5);
						sprintf(end_tag2, "</%s:%s ", prefix, name);
					}
					else {
						end_tag1 = (char *) xml_malloc(strlen(name) + 4);
						sprintf(end_tag1, "</%s>", name);
						end_tag2 = (char *) xml_malloc(strlen(name) + 4);
						sprintf(end_tag2, "</%s ", name);
					}

					if(xml_strstr(doc_front, end_tag1))
						doc_rear = xml_strstr(doc_front, end_tag1);
					else if(xml_strstr(doc_front, end_tag2))
						doc_rear = xml_strstr(doc_front, end_tag2);
					else
						doc_rear = NULL;

					if(doc_rear) {
						node = _xml_new_element(prefix, name, uri, attr);

						if(root) {
							xml_add_child(root, node);
							_xml_parse_doc(doc_front, doc_rear - doc_front, node);
							cur_pos = strchr(doc_rear, '>') + 1;
						}
						else {
							root = node;
							_xml_parse_doc(doc_front, doc_rear - doc_front, node);
							cur_pos = xml_buf + doc_len;
						}
					}
					else {
						cur_pos = doc_front;
					}

					xml_free(end_tag1);
					xml_free(end_tag2);
				}

				xml_free(start_tag);
				xml_free(name);
				if(prefix) xml_free(prefix);
				if(uri) xml_free(uri);
				if(attr) xml_free(attr);
			}
			else {
				if(root && !root->child && (strlen(cur_pos) > 0)) {
					node = xml_new_text(cur_pos);
					xml_add_child(root, node);
				}

				cur_pos = xml_buf + doc_len;
			}
		}
		else {
			if(root && !root->child && (strlen(cur_pos) > 0)) {
				node = xml_new_text(cur_pos);
				xml_add_child(root, node);
			}

			cur_pos = xml_buf + doc_len;
		}
	}

	xml_free(xml_buf);

	return root;
}

/* Note: xml_parse_doc can handle attribute only for namespace */
struct xml_node *xml_parse_doc(char *doc_buf, int doc_len, char *doc_prefix, char *doc_name, char *doc_uri)
{
	struct xml_node *root = NULL;
	char *xml_buf, *start_tag, *end_tag, *empty_tag, *front, *rear;

	xml_buf = (char *) xml_malloc(doc_len + 1);
	memcpy(xml_buf, doc_buf, doc_len);
	xml_buf[doc_len] = '\0';

	if(doc_prefix && doc_uri) {
		start_tag = (char *) xml_malloc(2 * strlen(doc_prefix) + strlen(doc_name) + strlen(doc_uri) + 14);
		sprintf(start_tag, "<%s:%s xmlns:%s=\"%s\">", doc_prefix, doc_name, doc_prefix, doc_uri);
		empty_tag = (char *) xml_malloc(2 * strlen(doc_prefix) + strlen(doc_name) + strlen(doc_uri) + 15);
		sprintf(empty_tag, "<%s:%s xmlns:%s=\"%s\"/>", doc_prefix, doc_name, doc_prefix, doc_uri);

	}
	else if(doc_prefix) {
		start_tag = (char *) xml_malloc(strlen(doc_prefix) + strlen(doc_name) + 4);
		sprintf(start_tag, "<%s:%s>", doc_prefix, doc_name);
		empty_tag = (char *) xml_malloc(strlen(doc_prefix) + strlen(doc_name) + 5);
		sprintf(empty_tag, "<%s:%s/>", doc_prefix, doc_name);
	}
	else if(doc_uri) {
		start_tag = (char *) xml_malloc(strlen(doc_name) + strlen(doc_uri) + 12);
		sprintf(start_tag, "<%s xmlns=\"%s\">", doc_name, doc_uri);
		empty_tag = (char *) xml_malloc(strlen(doc_name) + strlen(doc_uri) + 13);
		sprintf(empty_tag, "<%s xmlns=\"%s\"/>", doc_name, doc_uri);
	}
	else {
		start_tag = (char *) xml_malloc(strlen(doc_name) + 3);
		sprintf(start_tag, "<%s>", doc_name);
		empty_tag = (char *) xml_malloc(strlen(doc_name) + 4);
		sprintf(empty_tag, "<%s/>", doc_name);
	}

	if(doc_prefix) {
		end_tag = (char *) xml_malloc(strlen(doc_prefix) + strlen(doc_name) + 5);
		sprintf(end_tag, "</%s:%s>", doc_prefix, doc_name);
	}
	else {
		end_tag = (char *) xml_malloc(strlen(doc_name) + 4);
		sprintf(end_tag, "</%s>", doc_name);
	}
	
	//Root element with content
	if((front = xml_strstr(xml_buf, start_tag)) != NULL) {
		front += strlen(start_tag);

		if((rear = xml_strstr(front, end_tag)) != NULL) {
			int xml_len = rear - front;

			root = xml_new_element(doc_prefix, doc_name, doc_uri);
			_xml_parse_doc(front, xml_len, root);
		}
	}
	//Root element without content
	else if((front = xml_strstr(xml_buf, empty_tag)) != NULL) {
			root = xml_new_element(doc_prefix, doc_name, doc_uri);
	}

	xml_free(start_tag);
	xml_free(end_tag);
	xml_free(empty_tag);
	xml_free(xml_buf);

	return root;
}

struct xml_node *xml_parse(char *doc_buf, int doc_len)
{
	char *proc_inst, *comment, *pos, *prolog_end;

	/* Remove XML Prolog */
	pos = doc_buf;
	while(pos < (doc_buf + doc_len)) {
		if((proc_inst = xml_strstr(pos, "<?")) != NULL) {
			pos = strchr(proc_inst, '>') + 1;
		}
		else {
			proc_inst = pos;
			break;
		}
	}
	pos = doc_buf;
	while(pos < (doc_buf + doc_len)) {
		if((comment = xml_strstr(pos, "<!")) != NULL) {
			pos = strchr(comment, '>') + 1;
		}
		else {
			comment = pos;
			break;
		}
	}
	if(proc_inst > comment)
		prolog_end = proc_inst;
	else
		prolog_end = comment;

	return _xml_parse_doc(prolog_end, doc_buf + doc_len - prolog_end, NULL);
}

static struct xml_node *xml_new_node(void)
{
	struct xml_node *node;

	node = (struct xml_node *) xml_malloc(sizeof(struct xml_node));
	memset(node, 0, sizeof(struct xml_node));

	return node;
}

static struct xml_node *_xml_new_element(char *prefix, char *name, char *uri, char *attr)
{
	struct xml_node *node;

	node = xml_new_node();
	node->name = (char *) xml_malloc(strlen(name) + 1);
	strcpy(node->name, name);
	
	if(prefix) {
		node->prefix = (char *) xml_malloc(strlen(prefix) + 1);
		strcpy(node->prefix, prefix);
	}

	if(uri) {
		node->uri = (char *) xml_malloc(strlen(uri) + 1);
		strcpy(node->uri, uri);
	}

	if(attr) {
		node->attr = (char *) xml_malloc(strlen(attr) + 1);
		strcpy(node->attr, attr);
	}

	return node;	
}

struct xml_node *xml_new_element(char *prefix, char *name, char *uri)
{
	struct xml_node *node;
	char *attr = NULL;

	if(prefix && uri) {
		attr = (char *) xml_malloc(strlen(prefix) + strlen(uri) + 10);
		sprintf(attr, "xmlns:%s=\"%s\"", prefix, uri);
	}
	else if(uri) {
		attr = (char *) xml_malloc(strlen(uri) + 9);
		sprintf(attr, "xmlns=\"%s\"", uri);
	}

	node = _xml_new_element(prefix, name, uri, attr);

	if(attr)
		xml_free(attr);

	return node;
}

struct xml_node *xml_new_text(char *text)
{
	struct xml_node *node;
	char *text_buf;

	text_buf = (char *) xml_malloc(strlen(text) + 1);
	strcpy(text_buf, text);
	node = xml_new_node();
	node->text = text_buf;

	return node;
}

int xml_is_element(struct xml_node *node)
{
	int ret = 0;

	if((node->name != NULL) && (node->text == NULL))
		ret = 1;

	return ret;
}

int xml_is_text(struct xml_node *node)
{
	int ret = 0;

	if((node->name == NULL) && (node->text != NULL))
		ret = 1;

	return ret;
}

static void _xml_copy_tree(struct xml_node *root, struct xml_node *parent)
{
	struct xml_node *copy = NULL;

	if(xml_is_text(root)) {
		copy = xml_new_text(root->text);
	}
	else if(xml_is_element(root)) {
		struct xml_node *child = root->child;

		copy = _xml_new_element(root->prefix, root->name, root->uri, root->attr);

		while(child) {
			_xml_copy_tree(child, copy);
			child = child->next;
		}
	}

	if(copy)
		xml_add_child(parent, copy);
}

struct xml_node* xml_copy_tree(struct xml_node *root)
{
	struct xml_node *copy = NULL;

	if(xml_is_text(root)) {
		copy = xml_new_text(root->text);
	}
	else if(xml_is_element(root)) {
		struct xml_node *child = root->child;

		copy = _xml_new_element(root->prefix, root->name, root->uri, root->attr);

		while(child) {
			_xml_copy_tree(child, copy);
			child = child->next;
		}
	}

	return copy;
}

void xml_delete_tree(struct xml_node *root)
{
	if(root->name)
		xml_free(root->name);

	if(root->text)
		xml_free(root->text);

	if(root->prefix)
		xml_free(root->prefix);

	if(root->uri)
		xml_free(root->uri);

	if(root->attr)
		xml_free(root->attr);

	while(root->child)
		xml_delete_tree(root->child);

	if(root->prev) {
		root->prev->next = root->next;

		if(root->next)
			root->next->prev = root->prev;
	}
	else if(root->parent) {
		root->parent->child = root->next;

		if(root->next)
			root->next->prev = NULL;
	}

	xml_free(root);
}

void xml_add_child(struct xml_node *node, struct xml_node *child)
{
	if(xml_is_element(node)) {
		if(node->child) {
			struct xml_node *last_child = node->child;

			while(last_child->next != NULL)
				last_child = last_child->next;

			last_child->next = child;
			child->prev = last_child;
		}
		else {
			node->child = child;
		}

		child->parent = node;
	}
}

void xml_clear_child(struct xml_node *node)
{
	while(node->child)
		xml_delete_tree(node->child);
}

struct xml_node* xml_text_child(struct xml_node *node)
{
	struct xml_node *child = NULL;

	if(node->child) {
		if(xml_is_text(node->child))
			child = node->child;
	}

	return child;
}

void xml_set_text(struct xml_node *node, char *text)
{
	if(xml_is_text(node)) {
		char *text_buf = (char *) xml_malloc(strlen(text) + 1);
		strcpy(text_buf, text);
		xml_free(node->text);
		node->text = text_buf;
	}
}

static void _xml_element_count(struct xml_node *root, char *name, int *count)
{
	if(xml_is_element(root)) {
		struct xml_node *child = root->child;

		if(strcmp(root->name, name) == 0) {
			(*count) ++;
		}

		while(child) {
			_xml_element_count(child, name, count);
			child = child->next;
		}
	}
}

static int xml_element_count(struct xml_node *root, char *name)
{
	int count = 0;

	_xml_element_count(root, name, &count);

	return count;
}

static void _xml_find_element(struct xml_node *root, char *name, struct xml_node_set *node_set)
{
	if(xml_is_element(root)) {
		struct xml_node *child = root->child;

		if(strcmp(root->name, name) == 0) {
			node_set->node[node_set->count] = root;
			node_set->count ++;
		}

		while(child) {
			_xml_find_element(child, name, node_set);
			child = child->next;
		}
	}
}

struct xml_node_set* xml_find_element(struct xml_node *root, char *name)
{
	struct xml_node_set *node_set = NULL;
	int node_count;

	node_set = (struct xml_node_set *) xml_malloc(sizeof(struct xml_node_set));
	node_set->count = 0;
	node_count = xml_element_count(root, name);

	if(node_count)
		node_set->node = (struct xml_node **) xml_malloc(node_count * sizeof(struct xml_node *));
	else
		node_set->node = NULL;

	_xml_find_element(root, name, node_set);

	return node_set;
}

static void _xml_path_count(struct xml_node *root, char *path, int *count)
{
	if(xml_is_element(root)) {
		char *front = NULL, *rear = NULL;

		if((front = strchr(path, '/')) != NULL) {
			int prefix_len, name_len;
			char *prefix, *name, *prefix_char;
			int prefix_matched = 0, name_matched = 0;

			front ++;
			prefix_char = strchr(front, ':');

			if((rear = strchr(front, '/')) != NULL) {
				if(prefix_char && (prefix_char < rear)) {
					prefix_len = prefix_char - front;
					prefix = (char *) xml_malloc(prefix_len + 1);
					memcpy(prefix, front, prefix_len);
					prefix[prefix_len] = '\0';

					name_len = rear - (prefix_char + 1);
					name = (char *) xml_malloc(name_len + 1);
					memcpy(name, prefix_char + 1, name_len);
					name[name_len] = '\0';
				}
				else {
					prefix = NULL;
					name_len = rear - front;
					name = (char *) xml_malloc(name_len + 1);
					memcpy(name, front, name_len);
					name[name_len] = '\0';
				}

				if((!prefix && !root->prefix) ||
				   (prefix && root->prefix && (strcmp(root->prefix, prefix) == 0)))
					prefix_matched = 1;
				else
					prefix_matched = 0;

				if(strcmp(root->name, name) == 0)
					name_matched = 1;
				else
					name_matched = 0;

				if(prefix_matched && name_matched) {
					struct xml_node *child = root->child;

					while(child) {
						_xml_path_count(child, rear, count);
						child = child->next;
					}
				}
			}
			else {
				if(prefix_char) {
					prefix_len = prefix_char - front;
					prefix = (char *) xml_malloc(prefix_len + 1);
					memcpy(prefix, front, prefix_len);
					prefix[prefix_len] = '\0';

					name_len = strlen(path) - (prefix_char + 1 - path);
					name = (char *) xml_malloc(name_len + 1);
					memcpy(name, prefix_char + 1, name_len);
					name[name_len] = '\0';
				}
				else {
					prefix = NULL;
					name_len = strlen(path) - (front - path);
					name = (char *) xml_malloc(name_len + 1);
					memcpy(name, front, name_len);
					name[name_len] = '\0';
				}

				if((!prefix && !root->prefix) ||
				   (prefix && root->prefix && (strcmp(root->prefix, prefix) == 0)))
					prefix_matched = 1;
				else
					prefix_matched = 0;

				if(strcmp(root->name, name) == 0)
					name_matched = 1;
				else
					name_matched = 0;

				if(prefix_matched && name_matched)
					(*count) ++;
			}

			if(prefix) xml_free(prefix);
			xml_free(name);
		}
	}
}

static int xml_path_count(struct xml_node *root, char *path)
{
	int count = 0;

	_xml_path_count(root, path, &count);

	return count;
}

static void _xml_find_path(struct xml_node *root, char *path, struct xml_node_set *node_set)
{
	if(xml_is_element(root)) {
		char *front = NULL, *rear = NULL;

		if((front = strchr(path, '/')) != NULL) {
			int prefix_len, name_len;
			char *prefix, *name, *prefix_char;
			int prefix_matched = 0, name_matched = 0;

			front ++;
			prefix_char = strchr(front, ':');

			if((rear = strchr(front, '/')) != NULL) {
				if(prefix_char && (prefix_char < rear)) {
					prefix_len = prefix_char - front;
					prefix = (char *) xml_malloc(prefix_len + 1);
					memcpy(prefix, front, prefix_len);
					prefix[prefix_len] = '\0';

					name_len = rear - (prefix_char + 1);
					name = (char *) xml_malloc(name_len + 1);
					memcpy(name, prefix_char + 1, name_len);
					name[name_len] = '\0';
				}
				else {
					prefix = NULL;
					name_len = rear - front;
					name = (char *) xml_malloc(name_len + 1);
					memcpy(name, front, name_len);
					name[name_len] = '\0';
				}

				if((!prefix && !root->prefix) ||
				   (prefix && root->prefix && (strcmp(root->prefix, prefix) == 0)))
					prefix_matched = 1;
				else
					prefix_matched = 0;

				if(strcmp(root->name, name) == 0)
					name_matched = 1;
				else
					name_matched = 0;

				if(prefix_matched && name_matched) {
					struct xml_node *child = root->child;

					while(child) {
						_xml_find_path(child, rear, node_set);
						child = child->next;
					}
				}
			}
			else {
				if(prefix_char) {
					prefix_len = prefix_char - front;
					prefix = (char *) xml_malloc(prefix_len + 1);
					memcpy(prefix, front, prefix_len);
					prefix[prefix_len] = '\0';

					name_len = strlen(path) - (prefix_char + 1 - path);
					name = (char *) xml_malloc(name_len + 1);
					memcpy(name, prefix_char + 1, name_len);
					name[name_len] = '\0';
				}
				else {
					prefix = NULL;
					name_len = strlen(path) - (front - path);
					name = (char *) xml_malloc(name_len + 1);
					memcpy(name, front, name_len);
					name[name_len] = '\0';
				}

				if((!prefix && !root->prefix) ||
				   (prefix && root->prefix && (strcmp(root->prefix, prefix) == 0)))
					prefix_matched = 1;
				else
					prefix_matched = 0;

				if(strcmp(root->name, name) == 0)
					name_matched = 1;
				else
					name_matched = 0;

				if(prefix_matched && name_matched) {
					node_set->node[node_set->count] = root;
					node_set->count ++;
				}
			}

			if(prefix) xml_free(prefix);
			xml_free(name);
		}
	}
}

struct xml_node_set* xml_find_path(struct xml_node *root, char *path)
{
	struct xml_node_set *node_set = NULL;
	int node_count;

	node_set = (struct xml_node_set *) xml_malloc(sizeof(struct xml_node_set));
	node_set->count = 0;
	node_count = xml_path_count(root, path);

	if(node_count)
		node_set->node = (struct xml_node **) xml_malloc(node_count * sizeof(struct xml_node *));
	else
		node_set->node = NULL;

	_xml_find_path(root, path, node_set);

	return node_set;
}

void xml_delete_set(struct xml_node_set *node_set)
{
	if(node_set->node)
		xml_free(node_set->node);

	xml_free(node_set);
}

static int xml_tree_size(struct xml_node *root, int level, int space)
{
	int size = 0;
	int next_level = (level)?(level + 1):0;

	if(xml_is_text(root)) {
		size += strlen(root->text);
	}
	else if(xml_is_element(root)) {
		int start_size, end_size;
		struct xml_node *child = root->child;
		int is_element_child = 0;

		if(root->prefix && root->attr)
			/* <prefix:name attr> */
			start_size = strlen(root->prefix) + strlen(root->name) + strlen(root->attr) + 4;
		else if(root->prefix)
			/* <prefix:name> */
			start_size = strlen(root->prefix) + strlen(root->name) + 3;
		else if(root->attr)
			/* <name attr> */
			start_size = strlen(root->name) + strlen(root->attr) + 3;
		else
			/* <name> */
			start_size = strlen(root->name) + 2;

		size += start_size;

		while(child) {
			if(((is_element_child = xml_is_element(child)) == 1) && level) {
				size ++; /* /n */
				size += (level * space); /* space */
			}

			size += xml_tree_size(child, next_level, space);
			child = child->next;
		}

		if(is_element_child && level) {
			size ++; /* /n */
			size += ((level - 1) * space); /* space */
		}

		if(root->prefix)
			/* </prefix:name> */
			end_size = strlen(root->prefix) + strlen(root->name) + 4;
		else
			/* </name> */
			end_size = strlen(root->name) + 3;

		size += end_size;
	}

	return size;
}

static void _xml_dump_tree(struct xml_node *root, char *xml_buf, int level, int space)
{
	int next_level = (level)?(level + 1):0;

	if(xml_is_text(root)) {
		strcat(xml_buf, root->text);
	}
	else if(xml_is_element(root)) {
		struct xml_node *child = root->child;
		int is_element_child = 0;

		if(root->prefix && root->attr) {
			strcat(xml_buf, "<");
			strcat(xml_buf, root->prefix);
			strcat(xml_buf, ":");
			strcat(xml_buf, root->name);
			strcat(xml_buf, " ");
			strcat(xml_buf, root->attr);
			strcat(xml_buf, ">");
		}
		else if(root->prefix) {
			strcat(xml_buf, "<");
			strcat(xml_buf, root->prefix);
			strcat(xml_buf, ":");
			strcat(xml_buf, root->name);
			strcat(xml_buf, ">");
		}
		else if(root->attr) {
			strcat(xml_buf, "<");
			strcat(xml_buf, root->name);
			strcat(xml_buf, " ");
			strcat(xml_buf, root->attr);
			strcat(xml_buf, ">");
		}
		else {
			strcat(xml_buf, "<");
			strcat(xml_buf, root->name);
			strcat(xml_buf, ">");
		}

		while(child) {
			if(((is_element_child = xml_is_element(child)) == 1) && level) {
				char space_buf[11];
				int i;

				strcat(xml_buf, "\n");
				memset(space_buf, ' ', sizeof(space_buf));
				space_buf[space] = '\0';

				for(i = 0; i < level; i ++)
					strcat(xml_buf, space_buf);
			}

			_xml_dump_tree(child, xml_buf, next_level, space);
			child = child->next;
		}

		if(is_element_child && level) {
			char space_buf[11];
			int i;

			strcat(xml_buf, "\n");
			memset(space_buf, ' ', sizeof(space_buf));
			space_buf[space] = '\0';

			for(i = 0; i < (level - 1); i ++)
				strcat(xml_buf, space_buf);
		}

		if(root->prefix) {
			strcat(xml_buf, "</");
			strcat(xml_buf, root->prefix);
			strcat(xml_buf, ":");
			strcat(xml_buf, root->name);
			strcat(xml_buf, ">");
		}
		else {
			strcat(xml_buf, "</");
			strcat(xml_buf, root->name);
			strcat(xml_buf, ">");
		}
	}
}

char *xml_dump_tree(struct xml_node *root)
{
	int xml_size;
	char *xml_buf;

	xml_size = xml_tree_size(root, 0, 0);
	xml_buf = (char *) xml_malloc(xml_size + 1);
	memset(xml_buf, 0, xml_size + 1);
	_xml_dump_tree(root, xml_buf, 0, 0);

	return xml_buf;
}

char *xml_dump_tree_ex(struct xml_node *root, char *prolog, int new_line, int space)
{
	int xml_size;
	char *xml_buf;

	/* Max offset of 10 for each level */
	if(space > 10)
		space = 10;

	xml_size = xml_tree_size(root, 1, space);

	if(prolog && new_line) {
		xml_buf = (char *) xml_malloc(strlen(prolog) + xml_size + 2);
		memset(xml_buf, 0, strlen(prolog) + xml_size + 2);
		sprintf(xml_buf, "%s\n", prolog);
		_xml_dump_tree(root, xml_buf + strlen(prolog), new_line, space);
	}
	else if(prolog) {
		xml_buf = (char *) xml_malloc(strlen(prolog) + xml_size + 1);
		memset(xml_buf, 0, strlen(prolog) + xml_size + 1);
		strcpy(xml_buf, prolog);
		_xml_dump_tree(root, xml_buf + strlen(prolog), new_line, space);
	}
	else {
		xml_buf = (char *) xml_malloc(xml_size + 1);
		memset(xml_buf, 0, xml_size + 1);
		_xml_dump_tree(root, xml_buf, new_line, space);
	}

	return xml_buf;
}

void xml_set_attribute(struct xml_node *node, char *attr, char *value)
{
	char *ns_tag, *new_attr;

	if(node->prefix) {
		ns_tag = (char *) xml_malloc(strlen("xmlns:") + strlen(node->prefix) + 1);
		sprintf(ns_tag, "xmlns:%s", node->prefix);

		if(strcmp(ns_tag, attr) == 0) {
			if(node->uri) xml_free(node->uri);
			node->uri = (char *) xml_malloc(strlen(value) + 1);
			strcpy(node->uri, value);
		}

		xml_free(ns_tag);
	}
	else {
		ns_tag = "xmlns";

		if(strcmp(ns_tag, attr) == 0) {
			if(node->uri) xml_free(node->uri);
			node->uri = (char *) xml_malloc(strlen(value) + 1);
			strcpy(node->uri, value);
		}
	}

	/* attr="value" or attr='value' */
	new_attr = (char *) xml_malloc(strlen(attr) + strlen(value) + 4);

	if(strchr(value, '\"'))
		sprintf(new_attr, "%s=\'%s\'", attr, value);
	else
		sprintf(new_attr, "%s=\"%s\"", attr, value);

	if(node->attr) {
		char *attr1, *attr2, *attr_pos, *all_attr, *attr_p1 = NULL, *attr_p2 = NULL;
		int attr_existed = 0;

		attr1 = (char *) xml_malloc(strlen(attr) + 4);
		sprintf(attr1, " %s=\'", attr);
		attr2 = (char *) xml_malloc(strlen(attr) + 4);
		sprintf(attr2, " %s=\"", attr);

		if(((attr_pos = xml_strstr(node->attr, attr1)) != NULL) ||
		   (xml_strstr(node->attr, attr1 + 1) == node->attr)) {
			attr_existed = 1;

			if(attr_pos) {
				attr_p1 = str_strip(node->attr, attr_pos - node->attr);
				attr_p2 = str_strip(strchr(attr_pos + strlen(attr1), '\'') + 1,
				                    node->attr + strlen(node->attr) - (strchr(attr_pos + strlen(attr1), '\'') + 1));
			}
			else {
				attr_p1 = NULL;
				attr_p2 = str_strip(strchr(node->attr + strlen(attr1) - 1, '\'') + 1,
				                    node->attr + strlen(node->attr) - (strchr(node->attr + strlen(attr1) - 1, '\'') + 1));
			}
		}
		else if(((attr_pos = xml_strstr(node->attr, attr2)) != NULL) ||
		        (xml_strstr(node->attr, attr2 + 1) == node->attr)) {
			attr_existed = 1;

			if(attr_pos) {
				attr_p1 = str_strip(node->attr, attr_pos - node->attr);
				attr_p2 = str_strip(strchr(attr_pos + strlen(attr2), '\"') + 1,
				                    node->attr + strlen(node->attr) - (strchr(attr_pos + strlen(attr2), '\"') + 1));
			}
			else {
				attr_p1 = NULL;
				attr_p2 = str_strip(strchr(node->attr + strlen(attr2) - 1, '\"') + 1,
				                    node->attr + strlen(node->attr) - (strchr(node->attr + strlen(attr2) - 1, '\"') + 1));
			}
		}

		if(attr_p1 && attr_p2) {
			all_attr = (char *) xml_malloc(strlen(attr_p1) + strlen(new_attr) + strlen(attr_p2) + 3);
			sprintf(all_attr, "%s %s %s", attr_p1, new_attr, attr_p2);
		}
		else if(attr_p1) {
			all_attr = (char *) xml_malloc(strlen(attr_p1) + strlen(new_attr) + 2);
			sprintf(all_attr, "%s %s", attr_p1, new_attr);
		}
		else if(attr_p2) {
			all_attr = (char *) xml_malloc(strlen(new_attr) + strlen(attr_p2) + 2);
			sprintf(all_attr, "%s %s", new_attr, attr_p2);
		}
		else if(attr_existed) {
			all_attr = (char *) xml_malloc(strlen(new_attr) + 1);
			sprintf(all_attr, "%s", new_attr);
		}
		else {
			all_attr = (char *) xml_malloc(strlen(node->attr) + strlen(new_attr) + 2);
			sprintf(all_attr, "%s %s", node->attr, new_attr);
		}

		xml_free(attr1);
		xml_free(attr2);
		if(attr_p1) xml_free(attr_p1);
		if(attr_p2) xml_free(attr_p2);
		xml_free(new_attr);
		xml_free(node->attr);
		node->attr = all_attr;
	}
	else {
		node->attr = new_attr;
	}
}

char *xml_get_attribute(struct xml_node *node, char *attr)
{
	char *value = NULL;

	if(node->attr) {
		/* attr=' or attr=" */
		char *value_front, *value_rear, *attr1, *attr2, *attr_pos;
		int value_len;

		attr1 = (char *) xml_malloc(strlen(attr) + 4);
		sprintf(attr1, " %s=\'", attr);
		attr2 = (char *) xml_malloc(strlen(attr) + 4);
		sprintf(attr2, " %s=\"", attr);

		if(((attr_pos = xml_strstr(node->attr, attr1)) != NULL) ||
		   (xml_strstr(node->attr, attr1 + 1) == node->attr)) {
			if(attr_pos)
				value_front = attr_pos + strlen(attr1);
			else
				value_front = node->attr + strlen(attr1) - 1;

			value_rear = strchr(value_front, '\'');
			value_len = value_rear - value_front;
			value = (char *) xml_malloc(value_len + 1);
			memcpy(value, value_front, value_len);
			value[value_len] = '\0';
		}
		else if(((attr_pos = xml_strstr(node->attr, attr2)) != NULL) ||
		        (xml_strstr(node->attr, attr2 + 1) == node->attr)) {
			if(attr_pos)
				value_front = attr_pos + strlen(attr2);
			else
				value_front = node->attr + strlen(attr2) - 1;

			value_rear = strchr(value_front, '\"');
			value_len = value_rear - value_front;
			value = (char *) xml_malloc(value_len + 1);
			memcpy(value, value_front, value_len);
			value[value_len] = '\0';
		}

		xml_free(attr1);
		xml_free(attr2);
	}

	return value;
}

