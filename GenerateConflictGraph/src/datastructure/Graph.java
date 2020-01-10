package datastructure;


/**
 * @author Rodrigo Prado
 *  
 */
public interface Graph extends Container
{
	// Methods
	
    void depthFirstTraversal(PrePostVisitor visitor, int start);

    void breadthFirstTraversal(Visitor visitor, int start);
	
	// Sets

    void addVertex(int v);

    void addVertex(int v, Object weight);

    void addEdge(int v, int w);

    void addEdge(int v, int w, Object weight);

    // Gets

    boolean isDirected();

    boolean isConnected();

    boolean isCyclic();
    
    int getNumberOfEdges();

    int getNumberOfVertices();

    Vertex getVertex(int v);

    Edge getEdge(int v, int w);

    boolean isEdge(int v, int w);

    Enumeration getVertices();

    Enumeration getEdges();
}
