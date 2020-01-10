package datastructure;

public class GeneralTree extends AbstractTree
{
	protected Object key;
	protected int degree;
	protected LinkedList list;

	public GeneralTree(Object key)
	{
		this.key = key;
		degree = 0;
		list = new LinkedList();
	}

	public void purge()
	{
		list.purge();
		degree = 0;
	}

	public Object getKey()
	{
		return key;
	}

	public Tree getSubtree(int i)
	{
		if (i < 0 || i >= degree)
			throw new IndexOutOfBoundsException();
		LinkedList.Element ptr = list.getHead();
		for (int j = 0; j < i; ++j)
			ptr = ptr.getNext();
		return (GeneralTree) ptr.getDatum();
	}

	public void attachSubtree(GeneralTree t)
	{
		list.append(t);
		++degree;
	}

	public GeneralTree detachSubtree(GeneralTree t)
	{
		list.extract(t);
		--degree;
		return t;
	}

	public boolean isEmpty()
	{
		return false;
	}

	public boolean isLeaf()
	{
		return degree == 0;
	}

	public int getDegree()
	{
		return degree;
	}

	protected int compareTo(Comparable comparable)
	{
		throw new MethodNotImplemented();
	}
}
