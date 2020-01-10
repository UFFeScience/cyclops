package datastructure;

public class BTree extends MWayTree
{
	protected BTree parent;

	public BTree(int m)
	{
		super(m);
	}

	public void attachSubtree(int i, MWayTree arg)
	{
		BTree btree = (BTree) arg;
		subtree[i] = btree;
		btree.parent = this;
	}

	public void insert(Comparable object)
	{
		if (isEmpty())
		{
			if (parent == null)
			{
				attachSubtree(0, new BTree(getM()));
				key[1] = object;
				attachSubtree(1, new BTree(getM()));
				count = 1;
			}
			else
				parent.insertPair(object, new BTree(getM()));
		}
		else
		{
			int index = findIndex(object);
			if (index != 0 && object.isEQ(key[index]))
				throw new IllegalArgumentException("duplicate key");
			subtree[index].insert(object);
		}
	}

	protected void insertPair(Comparable object, BTree child)
	{
		int index = findIndex(object);
		if (!isFull())
		{
			insertKey(index + 1, object);
			insertSubtree(index + 1, child);
			++count;
		}
		else
		{
			Comparable extraKey = insertKey(index + 1, object);
			BTree extraTree = insertSubtree(index + 1, child);
			if (parent == null)
			{
				BTree left = new BTree(getM());
				BTree right = new BTree(getM());
				left.attachLeftHalfOf(this);
				right.attachRightHalfOf(this);
				right.insertPair(extraKey, extraTree);
				attachSubtree(0, left);
				key[1] = key[(getM() + 1) / 2];
				attachSubtree(1, right);
				count = 1;
			}
			else
			{
				count = (getM() + 1) / 2 - 1;
				BTree right = new BTree(getM());
				right.attachRightHalfOf(this);
				right.insertPair(extraKey, extraTree);
				parent.insertPair(key[(getM() + 1) / 2], right);
			}
		}
	}

	protected Comparable insertKey(int i, Comparable comparable)
	{
		if (i == getM())
			return comparable;
		Comparable comparable1 = super.key[getM() - 1];
		for (int j = getM() - 1; j > i; j--)
			super.key[j] = super.key[j - 1];

		super.key[i] = comparable;
		return comparable1;
	}

	protected BTree insertSubtree(int i, BTree btree)
	{
		if (i == getM())
			return btree;
		BTree btree1 = (BTree) super.subtree[getM() - 1];
		for (int j = getM() - 1; j > i; j--)
			super.subtree[j] = super.subtree[j - 1];

		super.subtree[i] = btree;
		btree.parent = this;
		return btree1;
	}

	protected void attachLeftHalfOf(BTree btree)
	{
		super.count = (getM() + 1) / 2 - 1;
		attachSubtree(0, ((MWayTree) (btree)).subtree[0]);
		for (int i = 1; i <= super.count; i++)
		{
			super.key[i] = ((MWayTree) (btree)).key[i];
			attachSubtree(i, ((MWayTree) (btree)).subtree[i]);
		}

	}

	protected void attachRightHalfOf(BTree btree)
	{
		super.count = getM() - (getM() + 1) / 2 - 1;
		int i = (getM() + 1) / 2;
		attachSubtree(0, ((MWayTree) (btree)).subtree[i++]);
		for (int j = 1; j <= super.count;)
		{
			super.key[j] = ((MWayTree) (btree)).key[i];
			attachSubtree(j, ((MWayTree) (btree)).subtree[i]);
			j++;
			i++;
		}

	}

	public void withdraw(Comparable comparable)
	{
		throw new MethodNotImplemented();
	}

}
