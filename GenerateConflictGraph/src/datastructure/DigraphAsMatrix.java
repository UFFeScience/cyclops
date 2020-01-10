package datastructure;

import java.util.NoSuchElementException;

public class DigraphAsMatrix extends GraphAsMatrix implements Digraph {
	public DigraphAsMatrix(int i) {
		super(i);
	}

	protected void addEdge(Edge edge) {
		int i = edge.getV0().getNumber();
		int j = edge.getV1().getNumber();
		if (super.matrix[i][j] != null) {
			throw new IllegalArgumentException("duplicate edge");
		} else {
			super.matrix[i][j] = edge;
			super.numberOfEdges++;
			return;
		}
	}

	public Enumeration getEdges() {
		return new Enumeration() {

			public boolean hasMoreElements() {
				return v < numberOfVertices && w < numberOfVertices;
			}

			public Object nextElement() {
				if (v >= numberOfVertices || w >= numberOfVertices)
					throw new NoSuchElementException();
				Edge edge = matrix[v][w];
				for (w++; w < numberOfVertices; w++)
					if (matrix[v][w] != null)
						return edge;

				for (v++; v < numberOfVertices; v++)
					for (w = 0; w < numberOfVertices; w++)
						if (matrix[v][w] != null)
							return edge;

				return edge;
			}

			protected int v;
			protected int w;

			{
				label0: for (v = 0; v < numberOfVertices; v++)
					for (w = 0; w < numberOfVertices; w++)
						if (matrix[v][w] != null)
							break label0;

			}
		};
	}
}
