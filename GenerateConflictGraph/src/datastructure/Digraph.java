package datastructure;


/**
 * @author Rodrigo Prado
 *
 */
public interface Digraph extends Graph
{
    boolean isStronglyConnected();

    void topologicalOrderTraversal(Visitor visitor);
}
