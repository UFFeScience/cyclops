package datastructure;

public class NaryTree extends AbstractTree
{
	protected Object key;
	protected int degree;
	protected NaryTree[] subtree;

	public NaryTree(int degree)
	{
		key = null;
		this.degree = degree;
		subtree = null;
	}

	public NaryTree(int degree, Object key)
	{
		this.key = key;
		this.degree = degree;
		subtree = new NaryTree[degree];
		for (int i = 0; i < degree; ++i)
			subtree[i] = new NaryTree(degree);
	}

	public boolean isEmpty()
	{
		return key == null;
	}

	public Object getKey()
	{
		if (isEmpty())
			throw new InvalidOperationException();
		return key;
	}

	public void attachKey(Object object)
	{
		if (!isEmpty())
			throw new InvalidOperationException();
		key = object;
		subtree = new NaryTree[degree];
		for (int i = 0; i < degree; ++i)
			subtree[i] = new NaryTree(degree);
	}

	public Object detachKey()
	{
		if (!isLeaf())
			throw new InvalidOperationException();
		Object result = key;
		key = null;
		subtree = null;
		return result;
	}

	public Tree getSubtree(int i)
	{
		if (isEmpty())
			throw new InvalidOperationException();
		return subtree[i];
	}

	public void attachSubtree(int i, NaryTree t)
	{
		if (isEmpty() || !subtree[i].isEmpty())
			throw new InvalidOperationException();
		subtree[i] = t;
	}

	NaryTree detachSubtree(int i)
	{
		if (isEmpty())
			throw new InvalidOperationException();
		NaryTree result = subtree[i];
		subtree[i] = new NaryTree(degree);
		return result;
	}

	public void purge()
	{
		key = null;
		subtree = null;
	}

	public int getDegree()
	{
		return isEmpty() ? 0 : degree;
	}

	public boolean isLeaf()
	{
		if (isEmpty())
			return false;
		for (int i = 0; i < degree; i++)
			if (!subtree[i].isEmpty())
				return false;

		return true;
	}

	protected int compareTo(Comparable comparable)
	{
		throw new MethodNotImplemented();
	}
}
