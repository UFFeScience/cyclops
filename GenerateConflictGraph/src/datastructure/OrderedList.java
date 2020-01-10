package datastructure;

public interface OrderedList extends SearchableContainer
{
	Comparable get(int i);

	Cursor findPosition(Comparable object);
}
