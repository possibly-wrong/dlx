template<typename Row, typename Column>
struct dlx::DLX<Row, Column>::Node
{
    Node *L, *R, *U, *D, *column;
    int size;
    Row name;

    Node(const Row& name = Row()) :
        L(this), R(this), U(this), D(this), column(this),
        size(0),
        name(name) {}

    void insert_LR(Node *x) { L = x->L; R = x; x->L = x->L->R = this; }
    void insert_UD(Node *x) { U = x->U; D = x; x->U = x->U->D = this; }

    void remove_LR() { R->L = L; L->R = R; }
    void remove_UD() { D->U = U; U->D = D; }

    void restore_LR() { R->L = L->R = this; }
    void restore_UD() { D->U = U->D = this; }
};

template<typename Row, typename Column>
bool dlx::DLX<Row, Column>::search()
{
    bool running = true;
    Node *c = root->R;
    if (c == root)
    {
        return found();
    }
    for (Node *j = c->R; j != root; j = j->R)
    {
        if (j->size < c->size)
        {
            c = j;
        }
    }
    if (c->size == 0)
    {
        return stuck();
    }
    cover(c);
    for (Node *r = c->D; r != c && running; r = r->D)
    {
        solution.push_back(r->name);
        for (Node *j = r->R; j != r; j = j->R)
        {
            cover(j->column);
        }
        running = search();
        for (Node *j = r->L; j != r; j = j->L)
        {
            uncover(j->column);
        }
        solution.pop_back();
    }
    uncover(c);
    return running;
}

template<typename Row, typename Column>
void dlx::DLX<Row, Column>::cover(Node *c)
{
    c->remove_LR();
    for (Node *i = c->D; i != c; i = i->D)
    {
        for (Node *j = i->R; j != i; j = j->R)
        {
            j->remove_UD();
            j->column->size--;
        }
    }
}

template<typename Row, typename Column>
void dlx::DLX<Row, Column>::uncover(Node *c)
{
    for (Node *i = c->U; i != c; i = i->U)
    {
        for (Node *j = i->L; j != i; j = j->L)
        {
            j->column->size++;
            j->restore_UD();
        }
    }
    c->restore_LR();
}

template<typename Row, typename Column>
template<typename URNG>
dlx::Degrees dlx::DLX<Row, Column>::sample(URNG& g)
{
    Degrees degrees;
    for (Node *c = root->R; c->size > 0;)
    {
        for (Node *j = c->R; j != root; j = j->R)
        {
            if (j->size < c->size)
            {
                c = j;
            }
        }
        if (c->size > 0)
        {
            degrees.push_back(c->size);
            std::uniform_int_distribution<int> dist(1, c->size);
            int child = dist(g);
            for (int i = 0; i < child; ++i)
            {
                c = c->D;
            }
            push(c->name);
            c = root->R;
        }
    }
    for (auto it = degrees.begin(); it != degrees.end(); ++it)
    {
        pop();
    }
    return degrees;
}

///////////////////////////////////////////////////////////////////////////////
//
// Housekeeping creation/deletion of doubly-linked lists.
//

template<typename Row, typename Column>
dlx::DLX<Row, Column>::DLX() :
    root(new Node()),
    rows(),
    columns(),
    solution()
{
    // empty
}

template<typename Row, typename Column>
dlx::DLX<Row, Column>::~DLX()
{
    while (!solution.empty())
    {
        pop();
    }
    for (auto it = columns.begin(); it != columns.end(); ++it)
    {
        Node *r = it->second->D;
        while (r != it->second)
        {
            Node *next = r->D;
            delete r;
            r = next;
        }
        delete it->second;
    }
    delete root;
}

template<typename Row, typename Column>
void dlx::DLX<Row, Column>::add(const Row& row, const Column& col)
{
    Node *x = new Node(row);
    Node *&r = rows[row];
    if (r == 0)
    {
        r = x;
    }
    x->insert_LR(r);
    Node *&c = columns[col];
    if (c == 0)
    {
        c = new Node();
        c->insert_LR(root);
    }
    x->insert_UD(c);
    x->column = c;
    c->size++;
}

template<typename Row, typename Column>
void dlx::DLX<Row, Column>::optional(const Column& col)
{
    Node *&c = columns[col];
    if (c == 0)
    {
        c = new Node();
    }
    c->remove_LR();
    c->L = c->R = c;
}

template<typename Row, typename Column>
void dlx::DLX<Row, Column>::push(const Row& row)
{
    Node *&r = rows[row];
    if (r != 0)
    {
        cover(r->column);
        for (Node *j = r->R; j != r; j = j->R)
        {
            cover(j->column);
        }
    }
    solution.push_back(row);
}

template<typename Row, typename Column>
void dlx::DLX<Row, Column>::pop()
{
    if (!solution.empty())
    {
        Node *&r = rows[solution.back()];
        if (r != 0)
        {
            for (Node *j = r->L; j != r; j = j->L)
            {
                uncover(j->column);
            }
            uncover(r->column);
        }
        solution.pop_back();
    }
}
