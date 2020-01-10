package datastructure;

import java.util.NoSuchElementException;

/**
 * @author Rodrigo Prado
 * 
 */
public abstract class AbstractGraph extends AbstractContainer implements Graph {
	// properties

	protected int numberOfVertices;

	protected int numberOfEdges;

	protected Vertex[] vertex;

	// Constructors

	public AbstractGraph(int size) {
		vertex = new Vertex[size];
	}

	// Methods

	public void depthFirstTraversal(PrePostVisitor visitor, int start) {
		boolean[] visited = new boolean[numberOfVertices];

		for (int v = 0; v < numberOfVertices; ++v)
			visited[v] = false;

		depthFirstTraversal(visitor, vertex[start], visited);
	}

	private void depthFirstTraversal(PrePostVisitor visitor, Vertex v, boolean[] visited) {
		if (visitor.isDone())
			return;

		visitor.preVisit(v);

		visited[v.getNumber()] = true;

		Enumeration p = v.getSuccessors();

		while (p.hasMoreElements()) {
			Vertex to = (Vertex) p.nextElement();

			if (!visited[to.getNumber()])
				depthFirstTraversal(visitor, to, visited);
		}

		visitor.postVisit(v);
	}

	public void breadthFirstTraversal(Visitor visitor, int start) {
		boolean[] enqueued = new boolean[numberOfVertices];

		for (int v = 0; v < numberOfVertices; ++v)
			enqueued[v] = false;

		Queue queue = new QueueAsLinkedList();

		enqueued[start] = true;

		queue.enqueue(vertex[start]);

		while (!queue.isEmpty() && !visitor.isDone()) {
			Vertex v = (Vertex) queue.dequeue();

			visitor.visit(v);

			Enumeration p = v.getSuccessors();

			while (p.hasMoreElements()) {
				Vertex to = (Vertex) p.nextElement();

				if (!enqueued[to.getNumber()]) {
					enqueued[to.getNumber()] = true;
					queue.enqueue(to);
				}
			}
		}
	} // public void breadthFirstTraversal(Visitor visitor, int start)

	public void topologicalOrderTraversal(Visitor visitor) {
		int[] inDegree = new int[numberOfVertices];

		for (int v = 0; v < numberOfVertices; ++v)
			inDegree[v] = 0;

		Enumeration p = getEdges();

		while (p.hasMoreElements()) {
			Edge edge = (Edge) p.nextElement();

			Vertex to = edge.getV1();
			++inDegree[to.getNumber()];
		}

		Queue queue = new QueueAsLinkedList();

		for (int v = 0; v < numberOfVertices; ++v)
			if (inDegree[v] == 0)
				queue.enqueue(vertex[v]);

		while (!queue.isEmpty() && !visitor.isDone()) {
			Vertex v = (Vertex) queue.dequeue();

			visitor.visit(v);

			Enumeration q = v.getSuccessors();

			while (q.hasMoreElements()) {
				Vertex to = (Vertex) q.nextElement();

				if (--inDegree[to.getNumber()] == 0)
					queue.enqueue(to);
			}
		}
	} // public void topologicalOrderTraversal(Visitor visitor)

	public boolean isConnected() {
		final Counter counter = new Counter();

		PrePostVisitor visitor = new AbstractPrePostVisitor() {
			public void preVisit(Object object) {
				++counter.value;
			}
		};

		depthFirstTraversal(visitor, 0);

		return counter.value == numberOfVertices;
	}

	public boolean isStronglyConnected() {
		final Counter counter = new Counter();

		for (int v = 0; v < numberOfVertices; ++v) {
			counter.value = 0;

			PrePostVisitor visitor = new AbstractPrePostVisitor() {
				public void preVisit(Object object) {
					++counter.value;
				}
			};

			depthFirstTraversal(visitor, v);

			if (counter.value != numberOfVertices)
				return false;
		}

		return true;
	}

	public boolean isCyclic() {
		final Counter counter = new Counter();

		Visitor visitor = new AbstractVisitor() {
			public void visit(Object object) {
				++counter.value;
			}
		};

		topologicalOrderTraversal(visitor);

		return counter.value != numberOfVertices;
	}

	public void purge() {
		for (int i = 0; i < numberOfVertices; i++)
			vertex[i] = null;

		numberOfVertices = 0;
		numberOfEdges = 0;
	}

	public String toString() {
		final StringBuffer buffer = new StringBuffer();

		AbstractVisitor abstractvisitor = new AbstractVisitor() {
			public void visit(Object obj) {
				Vertex vertex1 = (Vertex) obj;

				buffer.append(vertex1 + "\n");

				Edge edge;

				for (Enumeration enumeration = vertex1.getEmanatingEdges(); enumeration.hasMoreElements(); buffer
						.append("    " + edge + "\n"))
					edge = (Edge) enumeration.nextElement();

			}
		};

		accept(abstractvisitor);

		return getClass().getName() + " {\n" + buffer + "}";
	}

	public void accept(Visitor visitor) {
		for (int i = 0; i < numberOfVertices; i++) {
			if (visitor.isDone())
				break;

			visitor.visit(vertex[i]);
		}

	}

	// Adds

	public final void addEdge(int i, int j, Object obj) {
		addEdge(((Edge) (new GraphEdge(i, j, obj))));
	}

	public final void addEdge(int i, int j) {
		addEdge(i, j, null);
	}

	protected void addVertex(Vertex v) {
		if (numberOfVertices == vertex.length)
			throw new ContainerFullException();

		if (v.getNumber() != numberOfVertices)
			throw new IllegalArgumentException("invalid vertex number");

		vertex[numberOfVertices] = v;
		numberOfVertices++;
	}

	public final void addVertex(int i, Object obj) {
		addVertex(((Vertex) (new GraphVertex(i, obj))));
	}

	public final void addVertex(int i) {
		addVertex(i, null);
	}

	// Gets

	public boolean isDirected() {
		return this instanceof Digraph;
	}

	public Vertex getVertex(int i) {
		Bounds.check(i, 0, numberOfVertices);

		return vertex[i];
	}

	public Enumeration getVertices() {
		return new Enumeration() {
			protected int v;

			{
				v = 0;
			}

			public boolean hasMoreElements() {
				return v < numberOfVertices;
			}

			public Object nextElement() {
				if (v >= numberOfVertices)
					throw new NoSuchElementException();
				else
					return vertex[v++];
			}
		};
	}

	public int getNumberOfVertices() {
		return numberOfVertices;
	}

	public int getNumberOfEdges() {
		return numberOfEdges;
	}

	public Enumeration getEnumeration() {
		return getVertices();
	}

	// Abstract Methods

	protected abstract Enumeration getIncidentEdges(int v);

	protected abstract Enumeration getEmanatingEdges(int v);

	protected abstract void addEdge(Edge edge);

	// Inner classes

	protected final class GraphVertex extends AbstractObject implements Vertex {
		// Properties

		protected int number;

		protected Object weight;

		// Constructors

		public GraphVertex(int n) {
			number = n;
		}

		public GraphVertex(int n, Object w) {
			this(n);
			weight = w;
		}

		// Get

		public int getNumber() {
			return number;
		}

		public Object getWeight() {
			return (weight == null) ? null : weight;
		}

		public Enumeration getIncidentEdges() {
			return AbstractGraph.this.getIncidentEdges(number);
		}

		public Enumeration getEmanatingEdges() {
			return AbstractGraph.this.getEmanatingEdges(number);
		}

		public Enumeration getPredecessors() {
			return new Enumeration() {
				private final Enumeration edges;

				{
					edges = getIncidentEdges();
				}

				public boolean hasMoreElements() {
					return edges.hasMoreElements();
				}

				public Object nextElement() {
					Edge edge = (Edge) edges.nextElement();
					return edge.getMate(GraphVertex.this);
				}
			};
		}

		public Enumeration getSuccessors() {
			return new Enumeration() {
				private final Enumeration edges;

				{
					edges = getEmanatingEdges();
				}

				public boolean hasMoreElements() {
					return edges.hasMoreElements();
				}

				public Object nextElement() {
					Edge edge = (Edge) edges.nextElement();

					return edge.getMate(GraphVertex.this);
				}
			};
		}

		public int hashCode() {
			int i = number;

			if (weight != null)
				i += weight.hashCode();

			return i;
		}

		protected int compareTo(Comparable object) {
			int diff = number - ((GraphVertex) object).number;

			return diff;
		}

		public String toString() {
			StringBuffer stringbuffer = new StringBuffer();

			stringbuffer.append("Vertex {" + number);

			if (weight != null)
				stringbuffer.append(", weight = " + weight);

			stringbuffer.append("}");

			return stringbuffer.toString();
		}
	} // protected final class GraphVertex extends AbstractObject implements Vertex

	protected final class GraphEdge extends AbstractObject implements Edge {
		// Properties

		protected int v0;

		protected int v1;

		protected Object weight;

		// Constructors
		/*
		 * public GraphEdge(int i, int j) { v0 = i; v1 = j; }
		 */
		public GraphEdge(int i, int j, Object w) {
			// this(i, j);
			v0 = i;
			v1 = j;
			weight = w;
		}

		// Methods

		public int hashCode() {
			int i = v0 * numberOfVertices + v1;

			if (weight != null)
				i += weight.hashCode();

			return i;
		}

		protected int compareTo(Comparable arg) {
			// TODO compareTo AbstractGraph
			throw new MethodNotImplemented();
		}

		// Sets
		/*
		 * public void setWeight(Object weight) { this.weight = weight; }
		 */
		// Gets

		public Vertex getV0() {
			return vertex[v0];
		}

		public Vertex getV1() {
			return vertex[v1];
		}

		public Object getWeight() {
			return weight;
		}

		public boolean isDirected() {
			return AbstractGraph.this.isDirected();
		}

		public Vertex getMate(Vertex v) {
			if (v.getNumber() == v0)
				return vertex[v1];

			if (v.getNumber() == v1)
				return vertex[v0];
			else
				throw new IllegalArgumentException("invalid vertex");
		}

		// toString

		public String toString() {
			StringBuffer stringbuffer = new StringBuffer();

			stringbuffer.append("Edge {" + v0);

			if (isDirected())
				stringbuffer.append("->" + v1);
			else
				stringbuffer.append("--" + v1);

			if (weight != null)
				stringbuffer.append(", weight = " + weight);

			stringbuffer.append("}");

			return stringbuffer.toString();
		}
	} // protected final class GraphEdge extends AbstractObject implements Edge

	protected static final class Counter {
		int value;

		protected Counter() {
			value = 0;
		}
	}

}
