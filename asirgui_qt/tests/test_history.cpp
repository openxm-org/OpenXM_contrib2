#include "core/CommandHistory.h"

#include <cassert>

int main()
{
    CommandHistory history;
    history.add("fctr(x^4-1);");
    history.add("2^31-1;");

    assert(history.previous() == "2^31-1;");
    assert(history.previous() == "fctr(x^4-1);");
    assert(history.next() == "2^31-1;");
    assert(history.next().isEmpty());

    return 0;
}
