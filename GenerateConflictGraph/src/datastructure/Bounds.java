package datastructure;

public class Bounds
{
    public Bounds()
    {
    }

    public static void check(int i, int j, int k)
    {
        if(i < j || i > (j + k) - 1)
            throw new IndexOutOfBoundsException();
        else
            return;
    }
}
