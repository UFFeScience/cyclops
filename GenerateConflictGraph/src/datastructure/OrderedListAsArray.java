package datastructure;

import java.util.NoSuchElementException;

public class OrderedListAsArray extends AbstractSearchableContainer implements
		OrderedList
{
	protected Comparable[] array;

	public OrderedListAsArray(int size)
	{
		array = new Comparable[size];
	}

	public void insert(Comparable object)
	{
		if (count == array.length)
			throw new ContainerFullException();
		array[count] = object;
		++count;
	}

	public boolean isMember(Comparable object)
	{
		for (int i = 0; i < count; ++i)
			if (array[i] == object)
				return true;
		return false;
	}

	public Comparable find(Comparable arg)
	{
		for (int i = 0; i < count; ++i)
			if (array[i].isEQ(arg))
				return array[i];
		return null;
	}

	public void withdraw(Comparable object)
	{
		if (count == 0)
			throw new ContainerEmptyException();
		int i = 0;
		while (i < count && array[i] != object)
			++i;
		if (i == count)
			throw new IllegalArgumentException("object not found");
		for (; i < count - 1; ++i)
			array[i] = array[i + 1];
		array[i] = null;
		--count;
	}

	protected class MyCursor implements Cursor
	{
		int offset;

		MyCursor(int offset)
		{
			this.offset = offset;
		}

		public Comparable getDatum()
		{
			if (offset < 0 || offset >= count)
				throw new IndexOutOfBoundsException();
			return array[offset];
		}

		public void insertAfter(Comparable object)
		{
			if (offset < 0 || offset >= count)
				throw new IndexOutOfBoundsException();
			if (count == array.length)
				throw new ContainerFullException();

			int insertPosition = offset + 1;

			for (int i = count; i > insertPosition; --i)
				array[i] = array[i - 1];
			array[insertPosition] = object;
			++count;
		}

		public void insertBefore(Comparable comparable)
		{
			if (offset < 0 || offset >= count)
				throw new IndexOutOfBoundsException();
			if (count == array.length)
				throw new ContainerFullException();
			int i = offset;
			for (int j = count; j > i; j--)
				array[j] = array[j - 1];

			array[i] = comparable;
			count++;
		}

		public void withdraw()
		{
			if (offset < 0 || offset >= count)
				throw new IndexOutOfBoundsException();
			if (count == 0)
				throw new ContainerEmptyException();

			int i = offset;
			while (i < count - 1)
			{
				array[i] = array[i + 1];
				++i;
			}
			array[i] = null;
			--count;
		}

	}

	public Cursor findPosition(Comparable object)
	{
		int i = 0;
		while (i < count && array[i].isNE(object))
			++i;
		return new MyCursor(i);
	}

	public Comparable get(int offset)
	{
		if (offset < 0 || offset >= count)
			throw new IndexOutOfBoundsException();
		return array[offset];
	}

	public void purge()
	{
		while (super.count > 0)
			array[--super.count] = null;
	}

	public boolean isFull()
	{
		return super.count == array.length;
	}

	public void accept(Visitor visitor)
	{
		for (int i = 0; i < super.count; i++)
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

			public boolean hasMoreElements()
			{
				return offset < getCount();
			}

			public Object nextElement()
			{
				if (offset >= getCount())
					throw new NoSuchElementException();
				else
					return array[offset++];
			}

			int offset;

			{
				offset = 0;
			}
		};
	}

	protected int compareTo(Comparable comparable)
	{
		throw new MethodNotImplemented();
	}

}
