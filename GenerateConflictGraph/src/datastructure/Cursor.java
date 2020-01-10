package datastructure;

public interface Cursor
{
	Comparable getDatum();

	void insertAfter(Comparable object);

	void insertBefore(Comparable object);

	void withdraw();
}
