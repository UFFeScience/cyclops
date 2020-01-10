package datastructure;

/**
 * @author Rodrigo Prado
 *
 */
public class AVLTree extends BinarySearchTree {
	// Properties

	protected int height;

	// Constructors

	public AVLTree() {
		height = -1;
	}

	// Methods

	protected void adjustHeight() {
		if (isEmpty())
			height = -1;
		else
			height = 1 + Math.max(left.getHeight(), right.getHeight());
	}

	protected void doLLRotation() {
		if (isEmpty())
			throw new InvalidOperationException();

		BinaryTree tmp = right;
		right = left;
		left = right.left;
		right.left = right.right;
		right.right = tmp;

		Object tmpObj = key;
		key = right.key;
		right.key = tmpObj;

		getRightAVL().adjustHeight();
		adjustHeight();
	}

	protected void doLRRotation() {
		if (isEmpty())
			throw new InvalidOperationException();

		getLeftAVL().doRRRotation();
		doLLRotation();
	}

	private void doRRRotation() {
		if (isEmpty())
			throw new InvalidOperationException();

		BinaryTree tmp = left;
		left = right;
		right = left.right;
		left.right = left.left;
		left.left = tmp;

		Object tmpObj = key;
		key = left.key;
		left.key = tmpObj;

		getLeftAVL().adjustHeight();
		adjustHeight();
	}

	private void doRLRotation() {
		if (isEmpty())
			throw new InvalidOperationException();

		getRightAVL().doLLRotation();

		doRRRotation();
	}

	protected void balance() {
		adjustHeight();

		if (getBalanceFactor() > 1) {
			if (getLeftAVL().getBalanceFactor() > 0)
				doLLRotation();
			else
				doLRRotation();
		} else if (getBalanceFactor() < -1) {
			if (getRightAVL().getBalanceFactor() < 0)
				doRRRotation();
			else
				doRLRotation();
		}
	}

	public void attachKey(Object object) {
		if (!isEmpty())
			throw new InvalidOperationException();

		key = object;
		left = new AVLTree();
		right = new AVLTree();
		height = 0;
	}

	public Object detachKey() {
		height = -1;
		return super.detachKey();
	}

	// Gets

	public int getHeight() {
		return height;
	}

	protected int getBalanceFactor() {
		if (isEmpty())
			return 0;
		else
			return left.getHeight() - right.getHeight();
	}

	private AVLTree getRightAVL() {
		return (AVLTree) right;
	}

	private AVLTree getLeftAVL() {
		return (AVLTree) left;
	}

} // public class AVLTree extends BinarySearchTree
