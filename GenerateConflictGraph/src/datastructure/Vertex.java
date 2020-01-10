package datastructure;

/**
 * @author Rodrigo Prado
 *
 */
public interface Vertex extends Comparable
{
    int getNumber();
    
    Object getWeight();
    
    Enumeration getIncidentEdges();
    
    Enumeration getEmanatingEdges();
    
    Enumeration getPredecessors();
    
    Enumeration getSuccessors();
}
