package datastructure;

public class PartitionAsForest extends AbstractSet implements Partition
{
	protected PartitionTree[] array;

	public PartitionAsForest(int n)
	{
		super(n);
		array = new PartitionTree[universeSize];
		for (int item = 0; item < universeSize; ++item)
			array[item] = new PartitionTree(item);
		count = universeSize;
	}

	protected class PartitionTree extends AbstractSet implements Set, Tree
	{
		protected int item;
		protected PartitionTree parent;
		protected int rank;

		public PartitionTree(int item)
		{
			super(PartitionAsForest.this.universeSize);
			this.item = item;
			parent = null;
			rank = 0;
			count = 1;
		}

		public void purge()
		{
			parent = null;
			rank = 0;
			super.count = 1;
		}

		protected void setCount(int i)
		{
			super.count = i;
		}

		protected boolean IsMemberOf(PartitionAsForest partitionasforest)
		{
			return PartitionAsForest.this == partitionasforest;
		}

		public int getHeight()
		{
			return rank;
		}

		public Object getKey()
		{
			return new Int(item);
		}

		protected int compareTo(Comparable comparable)
		{
			PartitionTree partitiontree = (PartitionTree) comparable;
			return item - partitiontree.item;
		}

		public int hashCode()
		{
			return item;
		}

		public String toString()
		{
			String s = "PartitionTree {" + item;
			if (parent != null)
				s = s + ", " + parent;
			return s + "}";
		}

		protected void insert(int i)
		{
			throw new InvalidOperationException();
		}

		protected void withdraw(int i)
		{
			throw new InvalidOperationException();
		}

		protected boolean isMember(int i)
		{
			throw new MethodNotImplemented();
		}

		public boolean isLeaf()
		{
			throw new MethodNotImplemented();
		}

		public boolean isEmpty()
		{
			return false;
		}

		public Tree getSubtree(int i)
		{
			throw new MethodNotImplemented();
		}

		public int getDegree()
		{
			throw new MethodNotImplemented();
		}

		public void depthFirstTraversal(PrePostVisitor prepostvisitor)
		{
			throw new MethodNotImplemented();
		}

		public void breadthFirstTraversal(Visitor visitor)
		{
			throw new MethodNotImplemented();
		}

		public Set union(Set set)
		{
			throw new MethodNotImplemented();
		}

		public Set intersection(Set set)
		{
			throw new MethodNotImplemented();
		}

		public Set difference(Set set)
		{
			throw new MethodNotImplemented();
		}

		public boolean isEQ(Set set)
		{
			throw new MethodNotImplemented();
		}

		public boolean isSubset(Set set)
		{
			throw new MethodNotImplemented();
		}

		public Enumeration getEnumeration()
		{
			throw new MethodNotImplemented();
		}
	}

	public Set find(int item)
	{
		PartitionTree ptr = array[item];
		while (ptr.parent != null)
			ptr = ptr.parent;
		return ptr;
	}

	protected void checkArguments(PartitionTree s, PartitionTree t)
	{
		if (!isMember(s) || s.parent != null || !isMember(t)
				|| t.parent != null || s == t)
			throw new IllegalArgumentException("incompatible sets");
	}

	public void join(Set s, Set t)
	{
		PartitionTree p = (PartitionTree) s;
		PartitionTree q = (PartitionTree) t;
		checkArguments(p, q);
		q.parent = p;
		--count;
	}
	

    public void purge()
    {
        for(int i = 0; i < super.universeSize; i++)
            array[i].purge();

    }

    public Comparable find(Comparable comparable)
    {
        return find(((Int)comparable).intValue());
    }

    public boolean isMember(Comparable comparable)
    {
        PartitionTree partitiontree = (PartitionTree)comparable;
        return partitiontree.IsMemberOf(this);
    }

    public void accept(Visitor visitor)
    {
        for(int i = 0; i < super.universeSize; i++)
        {
            visitor.visit(array[i]);
            if(visitor.isDone())
                return;
        }

    }

    protected void insert(int i)
    {
        throw new InvalidOperationException();
    }

    protected void withdraw(int i)
    {
        throw new InvalidOperationException();
    }

    protected boolean isMember(int i)
    {
        throw new InvalidOperationException();
    }

    public Enumeration getEnumeration()
    {
        throw new MethodNotImplemented();
    }

    public Set union(Set set)
    {
        throw new InvalidOperationException();
    }

    public Set intersection(Set set)
    {
        throw new InvalidOperationException();
    }

    public Set difference(Set set)
    {
        throw new InvalidOperationException();
    }

    public boolean isEQ(Set set)
    {
        throw new MethodNotImplemented();
    }

    public boolean isSubset(Set set)
    {
        throw new MethodNotImplemented();
    }

    protected int compareTo(Comparable comparable)
    {
        throw new MethodNotImplemented();
    }
}
