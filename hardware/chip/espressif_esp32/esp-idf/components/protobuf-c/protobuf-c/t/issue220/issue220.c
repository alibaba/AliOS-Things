#include <assert.h>
#include <stdlib.h>
#include <limits.h>

#include "t/issue220/issue220.pb-c.h"

int main(void)
{
	assert(_MESSAGE_TYPE1__FLAG_IS_INT_SIZE == INT_MAX);
	assert(_MESSAGE_TYPE2__ANOTHER_FLAG_IS_INT_SIZE == INT_MAX);
	assert(_TOP_LEVEL__SUBMESSAGES_IS_INT_SIZE == INT_MAX);
	return EXIT_SUCCESS;
}
