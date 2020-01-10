package datastructure;

import java.util.NoSuchElementException;

public class StackAsArray extends AbstractContainer implements Stack
{
	protected Object[] array;

	public StackAsArray(int size)
	{
		array = new Object[size];
	}

	public void purge()
	{
		while (count > 0)
			array[--count] = null;
	}

	public void push(Object object)
	{
		if (count == array.length)
			throw new ContainerFullException();
		array[count++] = object;
	}

	public Object pop()
	{
		if (count == 0)
			throw new ContainerEmptyException();
		Object result = array[--count];
		array[count] = null;
		return result;
	}

	public Object getTop()
	{
		if (count == 0)
			throw new ContainerEmptyException();
		return array[count - 1];
	}

	public void accept(Visitor visitor)
	{
		for (int i = 0; i < count; ++i)
		{
			visitor.visit(array[i]);
			if (visitor.isDone())
				return;
		}
	}

	public Enumeration getEnumeration()
	{
		return new Enumeration()
		{
			protected int position = 0;

			public boolean hasMoreElements()
			{
				return position < getCount();
			}

			public Object nextElement()
			{
				if (position >= getCount())
					throw new NoSuchElementException();
				return array[position++];
			}
		};
	}

	public boolean isFull()
	{
		return super.count == array.length;
	}

	protected int compareTo(Comparable comparable)
	{
		throw new MethodNotImplemented();
	}

}
