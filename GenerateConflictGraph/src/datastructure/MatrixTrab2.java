package datastructure;

public class MatrixTrab2
{
	// properties
	
    protected int numberOfRows;
    protected int numberOfColumns;
    protected Chr array[][];

    // Constructors
    
    public MatrixTrab2(int i, int j)
    {
        numberOfRows    = i;
        numberOfColumns = j;
        array           = new Chr[i][j];
    } // public MatrixTrab2(int i, int j)
    
    // Method
    
    // Get

    public Chr get(int i, int j)
    {
        return array[i][j];
    }
    
    // Set

    public void put(int i, int j, Chr d)
    {
        array[i][j] = d;
    }
    
} // public class MatrixTrab2
