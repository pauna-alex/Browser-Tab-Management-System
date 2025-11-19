# Browser Tab Management System

## Description of Auxiliary Functions

In addition to the main functions, auxiliary functions adapted from the Data Structures and Algorithms (SDA) laboratories were used. These were modified to comply with the specific structures used in the project (`page`, `tab`, `tabsList`, `stack`), discarding their generic nature. Examples include:

* **Linked List (`linked_list`)**: Functions such as `ll_create`, `ll_add_nth_node`, `ll_remove_nth_node`, and `ll_free` were adapted to manage the `struct page` structure within the history stacks.
* **Stack (`stack`)**: Functions such as `st_create`, `st_push`, `st_pop`, `st_peek`, and `st_free` were adjusted to store pages and correctly free memory, including page descriptions.
* **Tab List (`tabsList`)**: Functions such as `tabsList_create`, `tabsList_add_nth_node`, `tabsList_remove_nth_node`, and `tabsList_free` were modified to manage a circular list of tabs based on `struct tab`.
* **Other functions**: `get_nth_node` and `tabsList_get_node_by_id` were optimized to operate efficiently with the defined structures and project requirements.

## Description of Main Functions

The `read_commands` function retrieves commands from the input file and delegates them to the corresponding functions, each managing a specific operation within the browser. These are described below:

### 1. `new_tab(struct browser *google, int nr)`

* **Purpose**: Creates a new tab with a default page.
* **Parameters**:
    * `google`: The browser structure, containing the list of tabs and the current tab.
    * `nr`: The unique identifier of the new tab.
* **Functionality**:
    * Allocates memory for a new tab and a default page ("https://acs.pub.ro/" with the description "Computer Science").
    * Initializes empty history stacks (backward and forward).
    * Adds the tab to the circular list and sets it as the current tab.

### 2. `open(struct browser *google, FILE *out, FILE *in)`

* **Purpose**: Switches the current tab to one specified by ID.
* **Parameters**:
    * `google`: The browser structure.
    * `out`: The output file.
    * `in`: The input file.
* **Functionality**:
    * Reads the tab ID from the input file.
    * Searches for the tab in the circular list; if found, sets it as the current tab.
    * Otherwise, prints "403 Forbidden" to the output file.

### 3. `next(struct browser *google, FILE *out)`

* **Purpose**: Switches to the next tab in the circular list.
* **Parameters**:
    * `google`: The browser structure.
    * `out`: The output file.
* **Functionality**:
    * Identifies the current tab.
    * Sets the next tab as the current tab (if it is the last tab, it wraps around to the first).
    * If the tab list is empty, prints "403 Forbidden".

### 4. `prev(struct browser *google, FILE *out)`

* **Purpose**: Switches to the previous tab in the circular list.
* **Parameters**:
    * `google`: The browser structure.
    * `out`: The output file.
* **Functionality**:
    * Identifies the current tab.
    * Sets the previous tab as the current tab (if it is the first tab, it wraps around to the last).
    * If the tab list is empty, prints "403 Forbidden".

### 5. `add_page(struct browser *google, struct page *vector, FILE *out, FILE *in)`

* **Purpose**: Replaces the tab's current page with a new one, specified by ID.
* **Parameters**:
    * `google`: The browser structure.
    * `vector`: The array of pages read from the file.
    * `out`: The output file.
    * `in`: The input file.
* **Functionality**:
    * Reads the page ID from the input file.
    * Validates the ID and the existence of the page in the vector.
    * Creates a new page with data from the vector and sets it as the current page.
    * Adds the old page to the backward stack and clears the forward stack.
    * If the ID is invalid, prints "403 Forbidden".

### 6. `backward(struct browser *google, FILE *out)`

* **Purpose**: Navigates to the previous page in the current tab's history.
* **Parameters**:
    * `google`: The browser structure.
    * `out`: The output file.
* **Functionality**:
    * Checks if the backward stack contains pages.
    * Pops the page from the top of the backward stack and sets it as the current page.
    * Adds the previous current page to the forward stack.
    * If the backward stack is empty, prints "403 Forbidden".

### 7. `forward(struct browser *google, FILE *out)`

* **Purpose**: Navigates to the next page in the current tab's history.
* **Parameters**:
    * `google`: The browser structure.
    * `out`: The output file.
* **Functionality**:
    * Checks if the forward stack contains pages.
    * Pops the page from the top of the forward stack and sets it as the current page.
    * Adds the previous current page to the backward stack.
    * If the forward stack is empty, prints "403 Forbidden".

### 8. `close(struct browser *google, FILE *out)`

* **Purpose**: Closes the current tab and switches to another tab.
* **Parameters**:
    * `google`: The browser structure.
    * `out`: The output file.
* **Functionality**:
    * Prohibits closing the default tab (ID 0) and prints "403 Forbidden".
    * Removes the current tab from the circular list and frees the associated memory.
    * Sets the previous tab as the current tab (or the last tab, if the current tab was the first).

### 9. `print(struct browser *google, FILE *out)`

* **Purpose**: Prints the IDs of the tabs and the description of the current page.
* **Parameters**:
    * `google`: The browser structure.
    * `out`: The output file.
* **Functionality**:
    * Prints the tab IDs in circular order, starting with the current tab.
    * Prints the description of the current page of the current tab.

### 10. `printh(struct browser *google, FILE *out, FILE *in)`

* **Purpose**: Prints the history of a tab specified by ID.
* **Parameters**:
    * `google`: The browser structure.
    * `out`: The output file.
    * `in`: The input file.
* **Functionality**:
    * Reads the tab ID from the input file.
    * If the tab does not exist, prints "403 Forbidden".
    * Prints the URLs in the following order: forward stack (reversed, using an auxiliary vector), current page, backward stack (in order).
