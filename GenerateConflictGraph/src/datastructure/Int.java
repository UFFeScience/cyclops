package datastructure;

public class Int extends AbstractObject
{
	protected int value;

	public Int(int value)
	{
		this.value = value;
	}

	public int intValue()
	{
		return value;
	}

	protected int compareTo(Comparable object)
	{
		Int arg = (Int) object;
		long diff = (long) value - (long) arg.value;
		if (diff < 0)
			return -1;
		else if (diff > 0)
			return +1;
		else
			return 0;
	}

	public int hashCode()
	{
		return value;
	}

    public String toString()
    {
        return Integer.toString(value);
    }

}
