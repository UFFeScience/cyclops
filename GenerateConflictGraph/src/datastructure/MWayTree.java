package datastructure;

import java.util.NoSuchElementException;

public class MWayTree extends AbstractTree implements SearchTree
{
	protected Comparable key[];
	protected MWayTree subtree[];

	public MWayTree(int m)
	{
		if (m < 2)
			throw new IllegalArgumentException("invalid degree");
		key = new Comparable[m];
		subtree = new MWayTree[m];
	}

	int getM()
	{
		return subtree.length;
	}

	public void depthFirstTraversal(PrePostVisitor visitor)
	{
		if (!isEmpty())
		{
			for (int i = 0; i <= count + 1; ++i)
			{
				if (i >= 2)
					visitor.postVisit(key[i - 1]);
				if (i >= 1 && i <= count)
					visitor.inVisit(key[i]);
				if (i <= count - 1)
					visitor.preVisit(key[i + 1]);
				if (i <= count)
					subtree[i].depthFirstTraversal(visitor);
			}
		}
	}

	public Comparable find(Comparable object)
	{
		if (isEmpty())
			return null;
		int i;
		for (i = count; i > 0; --i)
		{
			int diff = object.compare(key[i]);
			if (diff == 0)
				return key[i];
			if (diff > 0)
				break;
		}
		return subtree[i].find(object);
	}

	protected int findIndex(Comparable object)
	{
		if (isEmpty() || object.isLT(key[1]))
			return 0;
		int left = 1;
		int right = count;
		while (left < right)
		{
			int middle = (left + right + 1) / 2;
			if (object.isLT(key[middle]))
				right = middle - 1;
			else
				left = middle;
		}
		return left;
	}

	public Comparable findV2(Comparable object)
	{
		if (isEmpty())
			return null;
		int index = findIndex(object);
		if (index != 0 && object.isEQ(key[index]))
			return key[index];
		else
			return subtree[index].find(object);
	}

	public void insert(Comparable object)
	{
		if (isEmpty())
		{
			subtree[0] = new MWayTree(getM());
			key[1] = object;
			subtree[1] = new MWayTree(getM());
			count = 1;
		}
		else
		{
			int index = findIndex(object);
			if (index != 0 && object.isEQ(key[index]))
				throw new IllegalArgumentException("duplicate key");
			if (!isFull())
			{
				for (int i = count; i > index; --i)
				{
					key[i + 1] = key[i];
					subtree[i + 1] = subtree[i];
				}
				key[index + 1] = object;
				subtree[index + 1] = new MWayTree(getM());
				++count;
			}
			else
				subtree[index].insert(object);
		}
	}

	public void withdraw(Comparable object)
	{
		if (isEmpty())
			throw new IllegalArgumentException("object not found");
		int index = findIndex(object);
		if (index != 0 && object.isEQ(key[index]))
		{
			if (!subtree[index - 1].isEmpty())
			{
				Comparable max = subtree[index - 1].findMax();
				key[index] = max;
				subtree[index - 1].withdraw(max);
			}
			else if (!subtree[index].isEmpty())
			{
				Comparable min = subtree[index].findMin();
				key[index] = min;
				subtree[index].withdraw(min);
			}
			else
			{
				--count;
				int i;
				for (i = index; i <= count; ++i)
				{
					key[i] = key[i + 1];
					subtree[i] = subtree[i + 1];
				}
				key[i] = null;
				subtree[i] = null;
				if (count == 0)
					subtree[0] = null;
			}
		}
		else
			subtree[index].withdraw(object);
	}

	public void purge()
	{
		for (int i = 1; i <= super.count; i++)
			key[i] = null;

		for (int j = 0; j <= super.count; j++)
			subtree[j] = null;

		super.count = 0;
	}

	public void breadthFirstTraversal(Visitor visitor)
	{
		QueueAsLinkedList queueaslinkedlist = new QueueAsLinkedList();
		if (!isEmpty())
			queueaslinkedlist.enqueue(this);
		while (!queueaslinkedlist.isEmpty())
		{
			MWayTree mwaytree = (MWayTree) queueaslinkedlist.dequeue();
			for (int i = 1; i <= mwaytree.getDegree() - 1; i++)
				visitor.visit(mwaytree.getKey(i));

			for (int j = 0; j <= mwaytree.getDegree() - 1; j++)
			{
				MWayTree mwaytree1 = (MWayTree) mwaytree.getSubtree(j);
				if (!mwaytree1.isEmpty())
					queueaslinkedlist.enqueue(mwaytree1);
			}

		}
	}

	public boolean isEmpty()
	{
		return super.count == 0;
	}

	public boolean isFull()
	{
		return super.count == getM() - 1;
	}

	public boolean isLeaf()
	{
		if (isEmpty())
			return false;
		for (int i = 0; i <= super.count; i++)
			if (!subtree[i].isEmpty())
				return false;

		return true;
	}

	public int getDegree()
	{
		if (super.count == 0)
			return 0;
		else
			return super.count + 1;
	}

	public Object getKey(int i)
	{
		if (isEmpty())
		{
			throw new InvalidOperationException();
		}
		else
		{
			Bounds.check(i, 1, super.count);
			return key[i];
		}
	}

	public Object getKey()
	{
		throw new InvalidOperationException();
	}

	public Tree getSubtree(int i)
	{
		if (isEmpty())
		{
			throw new InvalidOperationException();
		}
		else
		{
			Bounds.check(i, 0, super.count + 1);
			return subtree[i];
		}
	}

	public int getCount()
	{
		if (isEmpty())
			return 0;
		int i = super.count;
		for (int j = 0; j <= super.count; j++)
			i += subtree[j].getCount();

		return i;
	}

	public boolean isMember(Comparable comparable)
	{
		if (isEmpty())
			return false;
		int i;
		for (i = super.count; i > 0; i--)
		{
			if (comparable == key[i])
				return true;
			if (comparable.isGT(key[i]))
				break;
		}

		return subtree[i].isMember(comparable);
	}

	public Comparable findMin()
	{
		if (isEmpty())
			return null;
		if (subtree[0].isEmpty())
			return key[1];
		else
			return subtree[0].findMin();
	}

	public Comparable findMax()
	{
		if (isEmpty())
			return null;
		if (subtree[super.count].isEmpty())
			return key[super.count];
		else
			return subtree[super.count].findMax();
	}

	public Enumeration getEnumeration()
	{
		return new Enumeration()
		{

			public boolean hasMoreElements()
			{
				return !stack.isEmpty();
			}

			public Object nextElement()
			{
				if (stack.isEmpty())
					throw new NoSuchElementException();
				MWayTree mwaytree = (MWayTree) stack.getTop();
				Object obj = mwaytree.getKey(position++);
				if (position > mwaytree.getDegree() - 1)
				{
					MWayTree mwaytree1 = (MWayTree) stack.pop();
					for (int i = mwaytree1.getDegree() - 1; i >= 0; i--)
					{
						Tree tree = mwaytree1.getSubtree(i);
						if (!tree.isEmpty())
							stack.push(tree);
					}

					position = 1;
				}
				return obj;
			}

			protected int position;
			protected Stack stack;

			{
				position = 1;
				stack = new StackAsLinkedList();
				if (!isEmpty())
					stack.push(MWayTree.this);
			}
		};
	}

	protected int compareTo(Comparable comparable)
	{
		throw new MethodNotImplemented();
	}
}
