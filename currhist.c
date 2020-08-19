#include "hsh.h"

/**
 * currhist - controls the history list
 * @h: the head
 * @t: the tail
 * Return: EXIT_SUCCESS in success.
 */
int currhist(commhist_s **h, commhist_s **t)
{
static commhist_s *head, *tail;

	if (h == NULL)			/* If h is NULL, clear head */
		head = NULL;
	else if (*h == NULL)	/* If h points to a NULL, get the head*/
		*h = head;
	else
		head = *h;			/* Else sets the head */

	if (t == NULL)			/* If t is NULL, clear tail */
		tail = NULL;
	else if (*t == NULL)	/* If t points to a NULL, get the tail */
		*t = tail;
	else
		tail = *t;			/* Else sets the tail */

	return (EXIT_SUCCESS);
}
