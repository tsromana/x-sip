/*
** Made by	db0
** Contact	db0company@gmail.com
** Site		http://db0.fr/
*/

#include	<stdlib.h>
#include	"list.h"

t_node *	get_node_as_arg(t_list * list,
				bool (*match_node)(t_node *, void *),
				void * arg)
{
  if (list)
    {
      list->cur = list->begin;
      while (list->cur)
	{
	  if (match_node(list->cur, arg))
	    return (list->cur);
	  list->cur = list->cur->next;
	}
    }
  return (NULL);
}

