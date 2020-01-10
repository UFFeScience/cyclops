package datastructure;

public interface Set extends SearchableContainer
{
	Set union(Set set);

	Set intersection(Set set);

	Set difference(Set set);

	boolean isEQ(Set set);

	boolean isSubset(Set set);
}
