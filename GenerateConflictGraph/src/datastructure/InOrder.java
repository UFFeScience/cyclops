package datastructure;

/**
 * @author Rodrigo Prado
 *
 */
public class InOrder extends AbstractPrePostVisitor {
	// Properties

	protected Visitor visitor;

	// Methods

	public InOrder(Visitor visitor) {
		this.visitor = visitor;
	}

	public void inVisit(Object object) {
		visitor.visit(object);
	}

	// Get

	public boolean isDone() {
		return visitor.isDone();
	}
}
