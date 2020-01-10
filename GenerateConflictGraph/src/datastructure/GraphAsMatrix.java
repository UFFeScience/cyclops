package datastructure;
import java.util.NoSuchElementException;

public class GraphAsMatrix extends AbstractGraph
{
	// Properties
	
    protected Edge[][] matrix;

    // Constructors
    
    public GraphAsMatrix(int size)
    {
        super (size);
        matrix = new Edge[size][size];
    }

    // Methods

    public void purge()
    {
        for (int i = 0; i < super.numberOfVertices; i++)
        {
            for (int j = 0; j < super.numberOfVertices; j++)
                matrix[i][j] = null;
        }

        super.purge();
    }

    protected int compareTo(Comparable comparable) {
    	// TODO compareTo GraphAsMatrix
        throw new MethodNotImplemented();
    }
    
    // Sets

    protected void addEdge(Edge edge)
    {
        int i = edge.getV0().getNumber();
        int j = edge.getV1().getNumber();
        
        if (matrix[i][j] != null)
            throw new IllegalArgumentException("duplicate edge");
        
        if (i == j)
            throw new IllegalArgumentException("loops not allowed");
        
        matrix[i][j] = edge;
        matrix[j][i] = edge;
        
        super.numberOfEdges++;
    }
    
    // Gets

    protected Enumeration getIncidentEdges(int i)
    {
        final Vertex arg = getVertex(i);
        
        return new Enumeration() 
                {
                    protected int v;
                    
                    protected int w;
                    
                    {
                        w = arg.getNumber();
                        for (v = 0; v < numberOfVertices; v++)
                            if (matrix[v][w] != null)
                                break;
                    }
        
                    public boolean hasMoreElements()
                    {
                        return v < numberOfVertices;
                    }
        
                    public Object nextElement()
                    {
                        if (v >= numberOfVertices)
                            throw new NoSuchElementException();
                        
                        Edge edge = matrix[v][w];
                        
                        for (v++; v < numberOfVertices; v++)
                            if(matrix[v][w] != null)
                                break;
        
                        return edge;
                    }
                };
    }

    protected Enumeration getEmanatingEdges(int i)
    {
        final Vertex arg = getVertex(i);
        
        return new Enumeration() 
                {
                	protected int v;
                    
                    protected int w;
                    
                    {
                        v = arg.getNumber();
                        for(w = 0; w < numberOfVertices; w++)
                            if(matrix[v][w] != null)
                                break;
        
                    }
        
                    public boolean hasMoreElements()
                    {
                        return w < numberOfVertices;
                    }
        
                    public Object nextElement()
                    {
                        if (w >= numberOfVertices)
                            throw new NoSuchElementException();
                        
                        Edge edge = matrix[v][w];
                        
                        for (w++; w < numberOfVertices; w++)
                            if(matrix[v][w] != null)
                                break;
        
                        return edge;
                    }
                };
    }

    public Enumeration getEdges()
    {
        return new Enumeration() 
                {
                    protected int v;
                    
                    protected int w;
                    
                    {
label0:
                        for (v = 0; v < numberOfVertices; v++)
                            for (w = v + 1; w < numberOfVertices; w++)
                                if (matrix[v][w] != null)
                                    break label0;
                    }
                    
                    public boolean hasMoreElements()
                    {
                        return v < numberOfVertices && w < numberOfVertices;
                    }
        
                    public Object nextElement()
                    {
                        if (v >= numberOfVertices || w >= numberOfVertices)
                            throw new NoSuchElementException();
                        
                        Edge edge = matrix[v][w];
                        
                        for (w++; w < numberOfVertices; w++)
                            if (matrix[v][w] != null)
                                return edge;
        
                        for (v++; v < numberOfVertices; v++)
                            for (w = v + 1; w < numberOfVertices; w++)
                                if (matrix[v][w] != null)
                                    return edge;

                        return edge;
                    }
                };
    }
    
    public Edge getEdge(int i, int j)
    {
        Bounds.check(i, 0, super.numberOfVertices);
        Bounds.check(j, 0, super.numberOfVertices);
        
        if (matrix[i][j] == null)
            throw new IllegalArgumentException("edge not found");
        else
            return matrix[i][j];
    }


    public boolean isEdge(int i, int j)
    {
        Bounds.check(i, 0, super.numberOfVertices);
        Bounds.check(j, 0, super.numberOfVertices);
        
        return matrix[i][j] != null;
    }

    /*
    final protected class Weight extends Int
    {
        public Weight (int value)
        {
            super (value);
        }
    }
	*/
    

	public void depthFirstTraversal(MyVisitor visitor, int start) {
		boolean[] visited = new boolean[numberOfVertices];

		for (int v = 0; v < numberOfVertices; ++v)
			visited[v] = false;

		depthFirstTraversal(visitor, vertex[start], visited, 0);
	}

	private void depthFirstTraversal(MyVisitor visitor, Vertex v, boolean[] visited, int height) {
//		if (visitor.isDone())
//			return;

		visitor.visit(v, height);

		visited[v.getNumber()] = true;

		Enumeration p = v.getSuccessors();

		while (p.hasMoreElements()) {
			Vertex to = (Vertex) p.nextElement();

			if (!visited[to.getNumber()])
				depthFirstTraversal(visitor, to, visited, height + 1);
		}
	}

}
