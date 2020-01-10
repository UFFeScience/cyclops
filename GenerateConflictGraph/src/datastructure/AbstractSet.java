package datastructure;

public abstract class AbstractSet extends AbstractSearchableContainer
{
	protected int universeSize;

	public AbstractSet(int universeSize)
	{
		this.universeSize = universeSize;
	}

	protected abstract void insert(int i);

	protected abstract void withdraw(int i);

	protected abstract boolean isMember(int i);

	public void insert(Comparable object)
	{
		insert(((Int) object).intValue());
	}

	public void withdraw(Comparable object)
	{
		withdraw(((Int) object).intValue());
	}

	public boolean isMember(Comparable object)
	{
		return isMember(((Int) object).intValue());
	}

    public Comparable find(Comparable comparable)
    {
        throw new InvalidOperationException();
    }

}
