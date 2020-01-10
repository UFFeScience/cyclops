package datastructure;

public class PartitionAsForestV2 extends PartitionAsForest
{
	public PartitionAsForestV2(int i)
    {
        super(i);
    }
	
	public Set find(int item)
	{
		PartitionTree root = array[item];
		while (root.parent != null)
			root = root.parent;
		PartitionTree ptr = array[item];
		while (ptr.parent != null)
		{
			PartitionTree tmp = ptr.parent;
			ptr.parent = root;
			ptr = tmp;
		}
		return root;
	}

	public void join(Set s, Set t)
	{
		PartitionTree p = (PartitionTree) s;
		PartitionTree q = (PartitionTree) t;
		checkArguments(p, q);
		if (p.getCount() > q.getCount())
		{
			q.parent = p;
			p.setCount(p.getCount() + q.getCount());
		}
		else
		{
			p.parent = q;
			q.setCount(p.getCount() + q.getCount());
		}
		--count;
	}
}
