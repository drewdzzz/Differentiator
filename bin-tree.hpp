///@file
#ifndef INCLUDE_TREE_H
#define INCLUDE_TREE_H

#include <iostream>
#include <stdlib.h>
#include <cstring>
#include "operators.hpp"
#include "unary_functions.hpp"
#define $p getchar()

enum node_type
{
    QUANTITY,
    VARIABLE,
    OPERATOR,
    UN_FUNCTION
};

struct informative_value
{
    union
    {
        double value;
        char code;
    }data;
    
    char type;

    informative_value & operator = (const informative_value &other)
    {
        this -> data = other.data;
        this -> type  = other.type;
        return *this;
    }

    informative_value ():
        data ({}),
        type (0)
    {};
    
    informative_value (double value):
        data ({}),
        type (0)
    {
        data.value = value;
        type = QUANTITY;
    };
};

///@brief Enum with error codes for using my binary tree
enum ERR_CODE
{
    OK = 0,
    ALLOC_ERROR = 1,
    IS_NOT_FREE = 2,
    NOT_EXIST = 3,
    NOT_LEAF = 4
};


///@brief Template class tree
template <typename T>
class Tree_t
{
public:
    ///@brief Node struct
    struct Node_t
    {
        T node_data;
        Node_t *right;
        Node_t *left;
        Node_t *father;

        Node_t ():                                  //Конструктор
            right (nullptr),
            left (nullptr),
            father (nullptr),
            node_data ({})
        {};

        Node_t (double value):                                
            right (nullptr),
            left (nullptr),
            father (nullptr),
            node_data (value)
        {};
        
        Node_t (const Node_t &other)                          
        {
            this -> node_data = other.node_data;
            if (other.left)
            {
                this -> left = new Node_t ( *(other.left) );
                this -> left -> father = this;
            }
            else this -> left = nullptr;
            
            if (other.right)
            {
                this -> right = new Node_t ( *(other.right) );
                this -> right -> father = this;
            } 
            else this -> right = nullptr;         

        }




        Node_t * operator + (Node_t &other)
        {
            Node_t* new_node = new Node_t;
            new_node -> node_data.type = OPERATOR;
            new_node -> node_data.data.code = '+';
            new_node -> left = this;
                new_node -> left -> father = new_node;
            new_node -> right = &other;
                new_node -> right -> father = new_node;
            return new_node;
        }

        Node_t * operator - (Node_t &other)
        {
            Node_t* new_node = new Node_t;
            new_node -> node_data.type = OPERATOR;
            new_node -> node_data.data.code = '-';
            new_node -> left = this;
                new_node -> left -> father = new_node;
            new_node -> right = &other;
                new_node -> right -> father = new_node;
            return new_node;
        }

        Node_t * operator * (Node_t &other)
        {
            Node_t* new_node = new Node_t;
            new_node -> node_data.type = OPERATOR;
            new_node -> node_data.data.code = '*';
            new_node -> left = this;
                new_node -> left -> father = new_node;
            new_node -> right = &other;
                new_node -> right -> father = new_node;
            return new_node;
        }

        Node_t * operator / (Node_t &other)
        {
            Node_t* new_node = new Node_t;
            new_node -> node_data.type = OPERATOR;
            new_node -> node_data.data.code = '/';
            new_node -> left = this;
                new_node -> left -> father = new_node;
            new_node -> right = &other;
                new_node -> right -> father = new_node;
            return new_node;
        }

        Node_t * operator ^ (Node_t &other)
        {
            Node_t* new_node = new Node_t;
            new_node -> node_data.type = OPERATOR;
            new_node -> node_data.data.code = '^';
            new_node -> left = this;
                new_node -> left -> father = new_node;
            new_node -> right = &other;
                new_node -> right -> father = new_node;
            return new_node;
        }



        Node_t & operator = (const Node_t &other)  
        {
            this -> node_data = other.node_data;

            if (this -> left)
                delete this -> left;

            if (this -> right)
                delete this -> right;

            if (other.left)
            {
                this -> left = new Node_t ( *(other.left) );
                this -> left -> father = this;
            }

            if (other.right)
            {
                this -> right = new Node_t ( *(other.right) );
                this -> right -> father = this;
            }

            return *this;
        }





    } *head;

    ///@brief Constructor of Tree_t
    Tree_t ()
    {
        head = new Node_t;
    }

    ///@brief Copy constructor of Tree_t
    Tree_t (const Tree_t &other)
    {
        *(this -> head) = *(other.head);
    }

    Tree_t &operator= (const Tree_t &other)
    {
        //delete this -> head;
        *(this -> head) = *(other.head);
        return *this;
    }

    ///@brief Destructor of Tree_t
    ~Tree_t ()
    {
        free_tree (head);
    }

    ///@brief Function sets data to node
    ///@return OK if data was set or NOT_EXIST if node doesn't exist
    ERR_CODE set_data (Node_t *node, T data)
    {
        if (!node) return NOT_EXIST;
        node -> node_data = data;
        return OK;
    }

    ///@brief Function creates a right descedant
    ///@return OK if descedant was created, NOT_EXIST if node doesn't exist, IS_NOT_FREE if descedant was created before
    ERR_CODE make_right (Node_t *node, T data)
    {
        if (!node)         return NOT_EXIST;
        if (node -> right) return IS_NOT_FREE;

        node -> right = new Node_t;
        node -> right -> node_data = data;
        node -> right -> father = node;
        return OK;
    }

    ///@brief Function adds an existing right descedant
    ///@return OK if descedant was created, NOT_EXIST if node doesn't exist, IS_NOT_FREE if descedant was created before
    ERR_CODE add_right (Node_t *node, Node_t *new_son)
    {
        if (!node)         return NOT_EXIST;
        if (node -> right) return IS_NOT_FREE;

        node -> right = new_son;
        new_son -> father = node;
        return OK;
    }

    ///@brief Function creates a left descedant
    ///@return OK if descedant was created, NOT_EXIST if node doesn't exist, IS_NOT_FREE if descedant was created before
    ERR_CODE make_left (Node_t *node, T data)
    {
        if (!node)        return NOT_EXIST;
        if (node -> left) return IS_NOT_FREE;

        node -> left = new Node_t;
        node -> left -> node_data = data;
        node -> left -> father = node;
        return OK;
    }

    ///@brief Function adds an existing left descedant
    ///@return OK if descedant was created, NOT_EXIST if node doesn't exist, IS_NOT_FREE if descedant was created before
    ERR_CODE add_left (Node_t *node, Node_t *new_son)
    {
        if (!node)        return NOT_EXIST;
        if (node -> left) return IS_NOT_FREE;

        node -> left = new_son;
        node -> left -> father = node;
        return OK;
    }

    ///@brief Delets a leaf
    ///@return OK if leaf was deletead, NOT_EXIST if node doesn't exist, NOT_LEAF if node is not leaf
    ERR_CODE delete_leaf (Node_t **node)
    {
        if (!node)                               return NOT_EXIST;
        if ((*node) -> right || (*node) -> left) return NOT_LEAF;
        free (*node);
        *node = nullptr;
    }

    ///@brief VIRTUAL function for definition of nodes for dump
    virtual void define_for_draw (FILE* stream, Node_t *node, bool dump)
    {
        fprintf (stream, "\"tree_node%p\" [label = \"", node);
        write_data (stream, node -> node_data);
        if (dump)
            fprintf (stream," \n Addres: %p\n Left: %p \n Right: %p\n Father: %p", node, node -> left, node -> right, node -> father);
        fprintf (stream,"\"]\n");

        if (node -> left)
        {
            define_for_draw (stream, node -> left, dump);
        }
        if (node -> right)
        {
            define_for_draw (stream, node -> right, dump);
        }
    }

    ///@brief VIRTUAL function for drawing of nodes
    virtual void draw_nodes (FILE* stream, Node_t *node)
    {
        fprintf (stream, "\"tree_node%p\"\n", node);
        if (node -> left)
        {
            fprintf (stream,"\"tree_node%p\" -> ", node);
            draw_nodes (stream, node -> left);
        }
        if (node -> right)
        {
            fprintf (stream,"\"tree_node%p\" -> ", node);
            draw_nodes (stream, node -> right);
        }

    }


    ///@brief dump of tree including only data
    ///@note Use "open" as an arguement for instant opening of png file
    void draw ()
    {
        FILE* stream = fopen ("Tree_draw.dot", "w");
        fprintf (stream, "digraph Tree{\n");
        define_for_draw (stream, head, false);
        draw_nodes (stream, head);
        fprintf (stream,"\n}");
        fclose (stream);
        system  ("dot -Tpng Tree_draw.dot -o tree_draw.png");
    }

    void draw (const char* arg)
    {
        draw ();
        if ( strcmp (arg, "open") == 0 )
            system ("xdg-open tree_draw.png");
    }

    ///@brief dump of tree including: data, address of node and addresses of children
    ///@note Use "open" as an arguement for instant opening of png file
    void dump ()
    {
        FILE* stream = fopen ("Tree_dump.dot", "w");
        fprintf (stream, "digraph Tree{\n");
        define_for_draw (stream, head, true);
        draw_nodes (stream, head);
        fprintf (stream,"\n}");
        fclose (stream);
        system  ("dot -Tpng Tree_dump.dot -o tree_dump.png");
    }

    void dump (const char* arg)
    {
        dump ();
        if ( strcmp (arg, "open") == 0 )
            system ("xdg-open tree_dump.png");
    }

    ///@brief A function for destructor... and btw it's private so it's NOT UR BUSINESS
    static void free_tree (Node_t *tree)
    {
        if (tree)
        {
            free_tree (tree -> left );
            free_tree (tree -> right);
            delete tree;
        }
    }
protected:
    //-----------------------------------------------------------------------------------------------------
    //Overloads of print function for dumping or drawing
    //-----------------------------------------------------------------------------------------------------
    ///@brief Overloads of print function for dumping or drawing
    void write_data (FILE* stream, const int &value)
    {
        fprintf (stream, "%d", value);
    }

    void write_data (FILE* stream, const unsigned int &value)
    {
        fprintf (stream, "%u", value);
    }

    void write_data (FILE* stream, const long &value)
    {
        fprintf (stream, "%ld", value);
    }

    void write_data (FILE* stream, const unsigned long &value)
    {
        fprintf (stream, "%lu", value);
    }

    void write_data (FILE* stream, const long long &value)
    {
        fprintf (stream, "%lld", value);
    }

    void write_data (FILE* stream, const unsigned long long &value)
    {
        fprintf (stream, "%llu", value);
    }

    void write_data (FILE* stream, const char &value)
    {
        fprintf (stream, "%c", value);
    }

    void write_data (FILE* stream, char* value)
    {
        fprintf (stream, "%s", value);
    }

    void write_data (FILE* stream, const float &value)
    {
        fprintf (stream, "%f", value);
    }

    void write_data (FILE* stream, const double &value)
    {
        fprintf (stream, "%lf", value);
    }

    void write_data (FILE* stream, informative_value &value)
    {
        switch (value.type)
        {
            case OPERATOR:
                fprintf (stream, "%c", value.data.code );
                break;
            
            case UN_FUNCTION:
                fprintf (stream, "%s", get_un_func_by_code (value.data.code));
                break;

            case VARIABLE:
                fprintf (stream, "%c", value.data.code );
                break;

            case QUANTITY:
                fprintf (stream, "%lg", value.data.value);
                break;
        }
    }
};
#endif
