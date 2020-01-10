package datastructure;

public interface SortedList extends SearchableContainer
{
	Comparable get(int i);

	Cursor findPosition(Comparable object);
}
