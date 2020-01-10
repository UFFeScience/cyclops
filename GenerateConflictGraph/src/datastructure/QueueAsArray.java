package datastructure;

import java.util.NoSuchElementException;

public class QueueAsArray extends AbstractContainer implements Queue
{
	protected Object[] array;
	protected int head;
	protected int tail;

	public QueueAsArray(int size)
	{
		array = new Object[size];
		head = 0;
		tail = size - 1;
	}

	public void purge()
	{
		while (count > 0)
		{
			array[head] = null;
			if (++head == array.length)
				head = 0;
			--count;
		}
	}

	public Object getHead()
	{
		if (count == 0)
			throw new ContainerEmptyException();
		return array[head];
	}

	public void enqueue(Object object)
	{
		if (count == array.length)
			throw new ContainerFullException();
		if (++tail == array.length)
			tail = 0;
		array[tail] = object;
		++count;
	}

	public Object dequeue()
	{
		if (count == 0)
			throw new ContainerEmptyException();
		Object result = array[head];
		array[head] = null;
		if (++head == array.length)
			head = 0;
		--count;
		return result;
	}

	public boolean isFull()
	{
		return super.count == array.length;
	}

	public void accept(Visitor visitor)
	{
		int i = head;
		for (int j = 0; j < super.count; j++)
		{
			visitor.visit(array[i]);
			if (visitor.isDone())
				return;
			if (++i == array.length)
				i = 0;
		}

	}

	public Enumeration getEnumeration()
	{
		return new Enumeration()
		{

			public boolean hasMoreElements()
			{
				return count < getCount();
			}

			public Object nextElement()
			{
				if (count >= getCount())
					throw new NoSuchElementException();
				Object obj = array[position];
				if (++position == array.length)
					position = 0;
				count++;
				return obj;
			}

			protected int position;
			protected int count;

			{
				position = head;
			}
		};
	}

	protected int compareTo(Comparable comparable)
	{
		throw new MethodNotImplemented();
	}

}
