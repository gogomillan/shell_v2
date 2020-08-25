#include "hsh.h"

/**
 * _trim - Trim characteres at the beginning and end of a string
 * @str: String
 * @c: Character to trim
 * Return: A pointer to the new position to start the string
 */
char *_trim(char *str, char c)
{
	char *h = str, *t = str; /* Head and Tail of the string */

	if (str == NULL)		/* No string */
		return (str);
	if (_strlen(str) <= 0)	/* Empty string */
		return (str);

	while (*t++ != '\0')	/* Trim the char at the beginning */
		if (*h == c)
			h++;
	t--, t--;
	while (*t == c)			/* Trim the char at the end */
		*t-- = '\0';
	/* Return a pointer to the beginning */
	return (h);
}

/**
 * _hide_char - Change a character from ctoh to cio if there is a double quote
 * @str: The string
 * @ctoh: Char to hide
 * @cio: Char instead of ctoh
 */
void _hide_char(char *str, char ctoh, char cio)
{
	char *p1 = str, *p2 = NULL;

	/* Verify if a correct string */
	if (str == NULL)
		return;
	/* Look for an opening double quote */
	p1 = _strchr(p1, '"');
	if (p1 == NULL)
		return;
	/* Look for a closing double quote */
	p2 = _strchr((p1 + 1), '"');
	if (p2 == NULL)
		return;
	/* Look for the char to hide (ctoh) inside the quoting text */
	while (p1 != p2)
	{
		if (*p1 == ctoh)
			*p1 = cio;
		p1++;
	}
}
