package datastructure;

public interface Partition extends Set
{
	Set find(int item);

	void join(Set set1, Set set2);
}
