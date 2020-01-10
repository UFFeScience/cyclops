package datastructure;


/**
 * @author Rodrigo Prado
 *
 */
public interface Edge extends Comparable
{
    Vertex getV0();
    
    Vertex getV1();
    
    Object getWeight();
    
    boolean isDirected();
    
    Vertex getMate(Vertex vertex);
}
