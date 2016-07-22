/**
 * @file vantage_point_tree_impl.hpp
 */
#ifndef MLPACK_CORE_TREE_VANTAGE_POINT_TREE_VANTAGE_POINT_TREE_IMPL_HPP
#define MLPACK_CORE_TREE_VANTAGE_POINT_TREE_VANTAGE_POINT_TREE_IMPL_HPP

// In case it wasn't included already for some reason.
#include "vantage_point_tree.hpp"
#include <queue>

namespace mlpack {
namespace tree {

// Each of these overloads is kept as a separate function to keep the overhead
// from the two std::vectors out, if possible.
template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename BoundMetricType, typename...> class BoundType,
         template<typename SplitBoundType, typename SplitMatType, size_t...>
             class SplitType>
VantagePointTree<MetricType, StatisticType, MatType, BoundType, SplitType>::
VantagePointTree(
    const MatType& data,
    const size_t maxLeafSize) :
    left(NULL),
    right(NULL),
    parent(NULL),
    begin(0), /* This root node starts at index 0, */
    count(data.n_cols), /* and spans all of the dataset. */
    bound(data.n_rows),
    parentDistance(0), // Parent distance for the root is 0: it has no parent.
    dataset(new MatType(data)), // Copies the dataset.
    firstPointIsCentroid(false)
{
  // Do the actual splitting of this node.
  SplitType<BoundType<MetricType>, MatType> splitter;
  SplitNode(maxLeafSize, splitter);

  // Create the statistic depending on if we are a leaf or not.
  stat = StatisticType(*this);
}

template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename BoundMetricType, typename...> class BoundType,
         template<typename SplitBoundType, typename SplitMatType, size_t...>
             class SplitType>
VantagePointTree<MetricType, StatisticType, MatType, BoundType, SplitType>::
VantagePointTree(
    const MatType& data,
    std::vector<size_t>& oldFromNew,
    const size_t maxLeafSize) :
    left(NULL),
    right(NULL),
    parent(NULL),
    begin(0),
    count(data.n_cols),
    bound(data.n_rows),
    parentDistance(0), // Parent distance for the root is 0: it has no parent.
    dataset(new MatType(data)), // Copies the dataset.
    firstPointIsCentroid(false)
{
  // Initialize oldFromNew correctly.
  oldFromNew.resize(data.n_cols);
  for (size_t i = 0; i < data.n_cols; i++)
    oldFromNew[i] = i; // Fill with unharmed indices.

  // Now do the actual splitting.
  SplitType<BoundType<MetricType>, MatType> splitter;
  SplitNode(oldFromNew, maxLeafSize, splitter);

  // Create the statistic depending on if we are a leaf or not.
  stat = StatisticType(*this);
}

template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename BoundMetricType, typename...> class BoundType,
         template<typename SplitBoundType, typename SplitMatType, size_t...>
             class SplitType>
VantagePointTree<MetricType, StatisticType, MatType, BoundType, SplitType>::
VantagePointTree(
    const MatType& data,
    std::vector<size_t>& oldFromNew,
    std::vector<size_t>& newFromOld,
    const size_t maxLeafSize) :
    left(NULL),
    right(NULL),
    parent(NULL),
    begin(0),
    count(data.n_cols),
    bound(data.n_rows),
    parentDistance(0), // Parent distance for the root is 0: it has no parent.
    dataset(new MatType(data)), // Copies the dataset.
    firstPointIsCentroid(false)
{
  // Initialize the oldFromNew vector correctly.
  oldFromNew.resize(data.n_cols);
  for (size_t i = 0; i < data.n_cols; i++)
    oldFromNew[i] = i; // Fill with unharmed indices.

  // Now do the actual splitting.
  SplitType<BoundType<MetricType>, MatType> splitter;
  SplitNode(oldFromNew, maxLeafSize, splitter);

  // Create the statistic depending on if we are a leaf or not.
  stat = StatisticType(*this);

  // Map the newFromOld indices correctly.
  newFromOld.resize(data.n_cols);
  for (size_t i = 0; i < data.n_cols; i++)
    newFromOld[oldFromNew[i]] = i;
}

template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename BoundMetricType, typename...> class BoundType,
         template<typename SplitBoundType, typename SplitMatType, size_t...>
             class SplitType>
VantagePointTree<MetricType, StatisticType, MatType, BoundType, SplitType>::
VantagePointTree(MatType&& data, const size_t maxLeafSize) :
    left(NULL),
    right(NULL),
    parent(NULL),
    begin(0),
    count(data.n_cols),
    bound(data.n_rows),
    parentDistance(0), // Parent distance for the root is 0: it has no parent.
    dataset(new MatType(std::move(data))),
    firstPointIsCentroid(false)
{
  // Do the actual splitting of this node.
  SplitType<BoundType<MetricType>, MatType> splitter;
  SplitNode(maxLeafSize, splitter);

  // Create the statistic depending on if we are a leaf or not.
  stat = StatisticType(*this);
}

template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename BoundMetricType, typename...> class BoundType,
         template<typename SplitBoundType, typename SplitMatType, size_t...>
             class SplitType>
VantagePointTree<MetricType, StatisticType, MatType, BoundType, SplitType>::
VantagePointTree(
    MatType&& data,
    std::vector<size_t>& oldFromNew,
    const size_t maxLeafSize) :
    left(NULL),
    right(NULL),
    parent(NULL),
    begin(0),
    count(data.n_cols),
    bound(data.n_rows),
    parentDistance(0), // Parent distance for the root is 0: it has no parent.
    dataset(new MatType(std::move(data))),
    firstPointIsCentroid(false)
{
  // Initialize oldFromNew correctly.
  oldFromNew.resize(dataset->n_cols);
  for (size_t i = 0; i < dataset->n_cols; i++)
    oldFromNew[i] = i; // Fill with unharmed indices.

  // Now do the actual splitting.
  SplitType<BoundType<MetricType>, MatType> splitter;
  SplitNode(oldFromNew, maxLeafSize, splitter);

  // Create the statistic depending on if we are a leaf or not.
  stat = StatisticType(*this);
}

template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename BoundMetricType, typename...> class BoundType,
         template<typename SplitBoundType, typename SplitMatType, size_t...>
             class SplitType>
VantagePointTree<MetricType, StatisticType, MatType, BoundType, SplitType>::
VantagePointTree(
    MatType&& data,
    std::vector<size_t>& oldFromNew,
    std::vector<size_t>& newFromOld,
    const size_t maxLeafSize) :
    left(NULL),
    right(NULL),
    parent(NULL),
    begin(0),
    count(data.n_cols),
    bound(data.n_rows),
    parentDistance(0), // Parent distance for the root is 0: it has no parent.
    dataset(new MatType(std::move(data))),
    firstPointIsCentroid(false)
{
  // Initialize the oldFromNew vector correctly.
  oldFromNew.resize(dataset->n_cols);
  for (size_t i = 0; i < dataset->n_cols; i++)
    oldFromNew[i] = i; // Fill with unharmed indices.

  // Now do the actual splitting.
  SplitType<BoundType<MetricType>, MatType> splitter;
  SplitNode(oldFromNew, maxLeafSize, splitter);

  // Create the statistic depending on if we are a leaf or not.
  stat = StatisticType(*this);

  // Map the newFromOld indices correctly.
  newFromOld.resize(dataset->n_cols);
  for (size_t i = 0; i < dataset->n_cols; i++)
    newFromOld[oldFromNew[i]] = i;
}

template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename BoundMetricType, typename...> class BoundType,
         template<typename SplitBoundType, typename SplitMatType, size_t...>
             class SplitType>
VantagePointTree<MetricType, StatisticType, MatType, BoundType, SplitType>::
VantagePointTree(
    VantagePointTree* parent,
    const size_t begin,
    const size_t count,
    SplitType<BoundType<MetricType>, MatType>& splitter,
    const size_t maxLeafSize,
    bool firstPointIsCentroid) :
    left(NULL),
    right(NULL),
    parent(parent),
    begin(begin),
    count(count),
    bound(parent->Dataset().n_rows),
    dataset(&parent->Dataset()), // Point to the parent's dataset.
    firstPointIsCentroid(firstPointIsCentroid)
{
  // Perform the actual splitting.
  SplitNode(maxLeafSize, splitter);

  // Create the statistic depending on if we are a leaf or not.
  stat = StatisticType(*this);
}

template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename BoundMetricType, typename...> class BoundType,
         template<typename SplitBoundType, typename SplitMatType, size_t...>
             class SplitType>
VantagePointTree<MetricType, StatisticType, MatType, BoundType, SplitType>::
VantagePointTree(
    VantagePointTree* parent,
    const size_t begin,
    const size_t count,
    std::vector<size_t>& oldFromNew,
    SplitType<BoundType<MetricType>, MatType>& splitter,
    const size_t maxLeafSize,
    bool firstPointIsCentroid) :
    left(NULL),
    right(NULL),
    parent(parent),
    begin(begin),
    count(count),
    bound(parent->Dataset().n_rows),
    dataset(&parent->Dataset()),
    firstPointIsCentroid(firstPointIsCentroid)
{
  // Hopefully the vector is initialized correctly!  We can't check that
  // entirely but we can do a minor sanity check.
  assert(oldFromNew.size() == dataset->n_cols);

  // Perform the actual splitting.
  SplitNode(oldFromNew, maxLeafSize, splitter);

  // Create the statistic depending on if we are a leaf or not.
  stat = StatisticType(*this);
}

template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename BoundMetricType, typename...> class BoundType,
         template<typename SplitBoundType, typename SplitMatType, size_t...>
             class SplitType>
VantagePointTree<MetricType, StatisticType, MatType, BoundType, SplitType>::
VantagePointTree(
    VantagePointTree* parent,
    const size_t begin,
    const size_t count,
    std::vector<size_t>& oldFromNew,
    std::vector<size_t>& newFromOld,
    SplitType<BoundType<MetricType>, MatType>& splitter,
    const size_t maxLeafSize,
    bool firstPointIsCentroid) :
    left(NULL),
    right(NULL),
    parent(parent),
    begin(begin),
    count(count),
    bound(parent->Dataset()->n_rows),
    dataset(&parent->Dataset()),
    firstPointIsCentroid(firstPointIsCentroid)
{
  // Hopefully the vector is initialized correctly!  We can't check that
  // entirely but we can do a minor sanity check.
  Log::Assert(oldFromNew.size() == dataset->n_cols);

  // Perform the actual splitting.
  SplitNode(oldFromNew, maxLeafSize, splitter);

  // Create the statistic depending on if we are a leaf or not.
  stat = StatisticType(*this);

  // Map the newFromOld indices correctly.
  newFromOld.resize(dataset->n_cols);
  for (size_t i = 0; i < dataset->n_cols; i++)
    newFromOld[oldFromNew[i]] = i;
}

/**
 * Create a binary space tree by copying the other tree.  Be careful!  This can
 * take a long time and use a lot of memory.
 */
template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename BoundMetricType, typename...> class BoundType,
         template<typename SplitBoundType, typename SplitMatType, size_t...>
             class SplitType>
VantagePointTree<MetricType, StatisticType, MatType, BoundType, SplitType>::
VantagePointTree(
    const VantagePointTree& other) :
    left(NULL),
    right(NULL),
    parent(other.parent),
    begin(other.begin),
    count(other.count),
    bound(other.bound),
    stat(other.stat),
    parentDistance(other.parentDistance),
    furthestDescendantDistance(other.furthestDescendantDistance),
    // Copy matrix, but only if we are the root.
    dataset((other.parent == NULL) ? new MatType(*other.dataset) : NULL),
    firstPointIsCentroid(other.firstPointIsCentroid)
{
  // Create left and right children (if any).
  if (other.Left())
  {
    left = new VantagePointTree(*other.Left());
    left->Parent() = this; // Set parent to this, not other tree.
  }

  if (other.Right())
  {
    right = new VantagePointTree(*other.Right());
    right->Parent() = this; // Set parent to this, not other tree.
  }

  // Propagate matrix, but only if we are the root.
  if (parent == NULL)
  {
    std::queue<VantagePointTree*> queue;
    if (left)
      queue.push(left);
    if (right)
      queue.push(right);
    while (!queue.empty())
    {
      VantagePointTree* node = queue.front();
      queue.pop();

      node->dataset = dataset;
      if (node->left)
        queue.push(node->left);
      if (node->right)
        queue.push(node->right);
    }
  }
}

/**
 * Move constructor.
 */
template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename BoundMetricType, typename...> class BoundType,
         template<typename SplitBoundType, typename SplitMatType, size_t...>
             class SplitType>
VantagePointTree<MetricType, StatisticType, MatType, BoundType, SplitType>::
VantagePointTree(VantagePointTree&& other) :
    left(other.left),
    right(other.right),
    parent(other.parent),
    begin(other.begin),
    count(other.count),
    bound(std::move(other.bound)),
    stat(std::move(other.stat)),
    parentDistance(other.parentDistance),
    furthestDescendantDistance(other.furthestDescendantDistance),
    minimumBoundDistance(other.minimumBoundDistance),
    dataset(other.dataset),
    firstPointIsCentroid(other.firstPointIsCentroid)
{
  // Now we are a clone of the other tree.  But we must also clear the other
  // tree's contents, so it doesn't delete anything when it is destructed.
  other.left = NULL;
  other.right = NULL;
  other.begin = 0;
  other.count = 0;
  other.parentDistance = 0.0;
  other.furthestDescendantDistance = 0.0;
  other.minimumBoundDistance = 0.0;
  other.dataset = NULL;
}

/**
 * Initialize the tree from an archive.
 */
template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename BoundMetricType, typename...> class BoundType,
         template<typename SplitBoundType, typename SplitMatType, size_t...>
             class SplitType>
template<typename Archive>
VantagePointTree<MetricType, StatisticType, MatType, BoundType, SplitType>::
VantagePointTree(
    Archive& ar,
    const typename boost::enable_if<typename Archive::is_loading>::type*) :
    VantagePointTree() // Create an empty BinarySpaceTree.
{
  // We've delegated to the constructor which gives us an empty tree, and now we
  // can serialize from it.
  ar >> data::CreateNVP(*this, "tree");
}

/**
 * Deletes this node, deallocating the memory for the children and calling their
 * destructors in turn.  This will invalidate any pointers or references to any
 * nodes which are children of this one.
 */
template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename BoundMetricType, typename...> class BoundType,
         template<typename SplitBoundType, typename SplitMatType, size_t...>
             class SplitType>
VantagePointTree<MetricType, StatisticType, MatType, BoundType, SplitType>::
  ~VantagePointTree()
{
  delete left;
  delete right;

  // If we're the root, delete the matrix.
  if (!parent)
    delete dataset;
}

template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename BoundMetricType, typename...> class BoundType,
         template<typename SplitBoundType, typename SplitMatType, size_t...>
             class SplitType>
inline bool VantagePointTree<MetricType, StatisticType, MatType, BoundType,
                            SplitType>::IsLeaf() const
{
  return !left;
}

/**
 * Returns the number of children in this node.
 */
template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename BoundMetricType, typename...> class BoundType,
         template<typename SplitBoundType, typename SplitMatType, size_t...>
             class SplitType>
inline size_t VantagePointTree<MetricType, StatisticType, MatType, BoundType,
                              SplitType>::NumChildren() const
{
  if (left && right)
    return 2;
  if (left)
    return 1;

  return 0;
}

/**
 * Return a bound on the furthest point in the node from the center.  This
 * returns 0 unless the node is a leaf.
 */
template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename BoundMetricType, typename...> class BoundType,
         template<typename SplitBoundType, typename SplitMatType, size_t...>
             class SplitType>
inline
typename VantagePointTree<MetricType, StatisticType, MatType, BoundType,
    SplitType>::ElemType
VantagePointTree<MetricType, StatisticType, MatType, BoundType,
    SplitType>::FurthestPointDistance() const
{
  if (!IsLeaf())
    return 0.0;

  // Otherwise return the distance from the center to a corner of the bound.
  return bound.OuterRadius();
}

/**
 * Return the furthest possible descendant distance.  This returns the maximum
 * distance from the center to the edge of the bound and not the empirical
 * quantity which is the actual furthest descendant distance.  So the actual
 * furthest descendant distance may be less than what this method returns (but
 * it will never be greater than this).
 */
template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename BoundMetricType, typename...> class BoundType,
         template<typename SplitBoundType, typename SplitMatType, size_t...>
             class SplitType>
inline
typename VantagePointTree<MetricType, StatisticType, MatType, BoundType,
    SplitType>::ElemType
VantagePointTree<MetricType, StatisticType, MatType, BoundType,
    SplitType>::FurthestDescendantDistance() const
{
  return furthestDescendantDistance;
}

//! Return the minimum distance from the center to any bound edge.
template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename BoundMetricType, typename...> class BoundType,
         template<typename SplitBoundType, typename SplitMatType, size_t...>
             class SplitType>
inline
typename VantagePointTree<MetricType, StatisticType, MatType, BoundType,
    SplitType>::ElemType
VantagePointTree<MetricType, StatisticType, MatType, BoundType,
    SplitType>::MinimumBoundDistance() const
{
  return bound.OuterRadius();
}

/**
 * Return the specified child.
 */
template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename BoundMetricType, typename...> class BoundType,
         template<typename SplitBoundType, typename SplitMatType, size_t...>
             class SplitType>
inline VantagePointTree<MetricType, StatisticType, MatType, BoundType,
                       SplitType>&
    VantagePointTree<MetricType, StatisticType, MatType, BoundType,
                    SplitType>::Child(const size_t child) const
{
  if (child == 0)
    return *left;
  else
    return *right;
}

/**
 * Return the number of points contained in this node.
 */
template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename BoundMetricType, typename...> class BoundType,
         template<typename SplitBoundType, typename SplitMatType, size_t...>
             class SplitType>
inline size_t VantagePointTree<MetricType, StatisticType, MatType, BoundType,
                              SplitType>::NumPoints() const
{
  // Each left intermediate node contains exactly one point.
  // Each right intermediate node contains no points.
  if (firstPointIsCentroid && left)
    return 1;
  else if(left)
    return 0;

  // This is a leaf node.
  return count;
}

/**
 * Return the number of descendants contained in the node.
 */
template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename BoundMetricType, typename...> class BoundType,
         template<typename SplitBoundType, typename SplitMatType, size_t...>
             class SplitType>
inline size_t VantagePointTree<MetricType, StatisticType, MatType, BoundType,
                              SplitType>::NumDescendants() const
{
  return count;
}

/**
 * Return the index of a particular descendant contained in this node.
 */
template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename BoundMetricType, typename...> class BoundType,
         template<typename SplitBoundType, typename SplitMatType, size_t...>
             class SplitType>
inline size_t VantagePointTree<MetricType, StatisticType, MatType, BoundType,
                              SplitType>::Descendant(const size_t index) const
{
  return (begin + index);
}

/**
 * Return the index of a particular point contained in this node.
 */
template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename BoundMetricType, typename...> class BoundType,
         template<typename SplitBoundType, typename SplitMatType, size_t...>
             class SplitType>
inline size_t VantagePointTree<MetricType, StatisticType, MatType, BoundType,
                              SplitType>::Point(const size_t index) const
{
  return (begin + index);
}

template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename BoundMetricType, typename...> class BoundType,
         template<typename SplitBoundType, typename SplitMatType, size_t...>
             class SplitType>
void VantagePointTree<MetricType, StatisticType, MatType, BoundType, SplitType>::
    SplitNode(const size_t maxLeafSize,
              SplitType<BoundType<MetricType>, MatType>& splitter)
{
  // We need to expand the bounds of this node properly.
  if (parent)
  {
    bound.Center() = parent->firstPointIsCentroid ?
			dataset->col(parent->begin + 1) : dataset->col(parent->begin);
    bound.OuterRadius() = 0;
    bound.InnerRadius() = std::numeric_limits<ElemType>::max();
  }

  if (count > 0)
    bound |= dataset->cols(begin, begin + count - 1);

  VantagePointTree* tree = this;

  while (tree->Parent() != NULL)
  {
    tree->Parent()->Bound() |= tree->Bound();
	tree->Parent()->furthestDescendantDistance = 0.5 *
			tree->Parent()->Bound().Diameter();
    tree = tree->Parent();
  }
  // Calculate the furthest descendant distance.
  furthestDescendantDistance = 0.5 * bound.Diameter();

  // Now, check if we need to split at all.
  if (count <= maxLeafSize)
    return; // We can't split this.

  // splitCol denotes the two partitions of the dataset after the split. The
  // points on its left go to the left child and the others go to the right
  // child.
  size_t splitCol;

  // Split the node. The elements of 'data' are reordered by the splitting
  // algorithm. This function call updates splitCol.

  size_t splitBegin = begin;
  size_t splitCount = count;

  if (FirstPointIsCentroid())
  {
    splitBegin = begin + 1;
    splitCount = count - 1;
  }

  const bool split = splitter.SplitNode(bound, *dataset, splitBegin, splitCount,
      splitCol);

  // The node may not be always split. For instance, if all the points are the
  // same, we can't split them.
  if (!split)
    return;

  // Now that we know the split column, we will recursively split the children
  // by calling their constructors (which perform this splitting process).
  left = new VantagePointTree(this, splitBegin, splitCol - splitBegin, splitter,
      maxLeafSize, true);
  right = new VantagePointTree(this, splitCol,
      splitBegin + splitCount - splitCol, splitter, maxLeafSize, false);

  // Calculate parent distances for those two nodes.
  arma::vec center, leftCenter, rightCenter;
  Center(center);
  left->Center(leftCenter);
  right->Center(rightCenter);

  const ElemType leftParentDistance = MetricType::Evaluate(center, leftCenter);
  const ElemType rightParentDistance = MetricType::Evaluate(center,
      rightCenter);

  left->ParentDistance() = leftParentDistance;
  right->ParentDistance() = rightParentDistance;
}

template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename BoundMetricType, typename...> class BoundType,
         template<typename SplitBoundType, typename SplitMatType, size_t...>
             class SplitType>
void VantagePointTree<MetricType, StatisticType, MatType, BoundType, SplitType>::
SplitNode(std::vector<size_t>& oldFromNew,
          const size_t maxLeafSize,
          SplitType<BoundType<MetricType>, MatType>& splitter)
{
  // We need to expand the bounds of this node properly.

  if (parent)
  {
    bound.Center() = parent->firstPointIsCentroid ?
			dataset->col(parent->begin + 1) : dataset->col(parent->begin);
    bound.OuterRadius() = 0;
    bound.InnerRadius() = std::numeric_limits<ElemType>::max();
  }

  if (count > 0)
    bound |= dataset->cols(begin, begin + count - 1);

  VantagePointTree* tree = this;

  while (tree->Parent() != NULL)
  {
    tree->Parent()->Bound() |= tree->Bound();
	tree->Parent()->furthestDescendantDistance = 0.5 *
			tree->Parent()->Bound().Diameter();
    tree = tree->Parent();
  }

  // Calculate the furthest descendant distance.
  furthestDescendantDistance = 0.5 * bound.Diameter();

  // First, check if we need to split at all.
  if (count <= maxLeafSize)
    return; // We can't split this.

  // splitCol denotes the two partitions of the dataset after the split. The
  // points on its left go to the left child and the others go to the right
  // child.
  size_t splitCol;

  // Split the node. The elements of 'data' are reordered by the splitting
  // algorithm. This function call updates splitCol and oldFromNew.

  size_t splitBegin = begin;
  size_t splitCount = count;

  if (FirstPointIsCentroid())
  {
    splitBegin = begin + 1;
    splitCount = count - 1;
  }

  const bool split = splitter.SplitNode(bound, *dataset, splitBegin, splitCount,
      splitCol, oldFromNew);

  // The node may not be always split. For instance, if all the points are the
  // same, we can't split them.
  if (!split)
    return;

  // Now that we know the split column, we will recursively split the children
  // by calling their constructors (which perform this splitting process).
  left = new VantagePointTree(this, splitBegin, splitCol - splitBegin,
      oldFromNew, splitter, maxLeafSize, true);
  right = new VantagePointTree(this, splitCol,
      splitBegin + splitCount - splitCol, oldFromNew, splitter, maxLeafSize,
      false);

  
  // Calculate parent distances for those two nodes.
  ElemType parentDistance;
  if (firstPointIsCentroid)
  {
    assert(left->firstPointIsCentroid == true);

    parentDistance = MetricType::Evaluate(dataset->col(begin),
        dataset->col(left->begin));
  }
  else
  {
    arma::vec center;
    Center(center);

    assert(left->firstPointIsCentroid == true);

    parentDistance = MetricType::Evaluate(center, dataset->col(left->begin));
  }

  left->ParentDistance() = parentDistance;
  right->ParentDistance() = parentDistance;
}

// Default constructor (private), for boost::serialization.
template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename BoundMetricType, typename...> class BoundType,
         template<typename SplitBoundType, typename SplitMatType, size_t...>
             class SplitType>
VantagePointTree<MetricType, StatisticType, MatType, BoundType, SplitType>::
    VantagePointTree() :
    left(NULL),
    right(NULL),
    parent(NULL),
    begin(0),
    count(0),
    stat(*this),
    parentDistance(0),
    furthestDescendantDistance(0),
    dataset(NULL)
{
  // Nothing to do.
}

/**
 * Serialize the tree.
 */
template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename BoundMetricType, typename...> class BoundType,
         template<typename SplitBoundType, typename SplitMatType, size_t...>
             class SplitType>
template<typename Archive>
void VantagePointTree<MetricType, StatisticType, MatType, BoundType, SplitType>::
    Serialize(Archive& ar, const unsigned int /* version */)
{
  using data::CreateNVP;

  // If we're loading, and we have children, they need to be deleted.
  if (Archive::is_loading::value)
  {
    if (left)
      delete left;
    if (right)
      delete right;
    if (!parent)
      delete dataset;
  }

  ar & CreateNVP(parent, "parent");
  ar & CreateNVP(begin, "begin");
  ar & CreateNVP(count, "count");
  ar & CreateNVP(bound, "bound");
  ar & CreateNVP(stat, "statistic");
  ar & CreateNVP(parentDistance, "parentDistance");
  ar & CreateNVP(furthestDescendantDistance, "furthestDescendantDistance");
  ar & CreateNVP(dataset, "dataset");

  // Save children last; otherwise boost::serialization gets confused.
  ar & CreateNVP(left, "left");
  ar & CreateNVP(right, "right");
  ar & CreateNVP(firstPointIsCentroid, "firstPointIsCentroid");

  // Due to quirks of boost::serialization, if a tree is saved as an object and
  // not a pointer, the first level of the tree will be duplicated on load.
  // Therefore, if we are the root of the tree, then we need to make sure our
  // children's parent links are correct, and delete the duplicated node if
  // necessary.
  if (Archive::is_loading::value)
  {
    // Get parents of left and right children, or, NULL, if they don't exist.
    VantagePointTree* leftParent = left ? left->Parent() : NULL;
    VantagePointTree* rightParent = right ? right->Parent() : NULL;

    // Reassign parent links if necessary.
    if (left && left->Parent() != this)
      left->Parent() = this;
    if (right && right->Parent() != this)
      right->Parent() = this;

    // Do we need to delete the left parent?
    if (leftParent != NULL && leftParent != this)
    {
      // Sever the duplicate parent's children.  Ensure we don't delete the
      // dataset, by faking the duplicated parent's parent (that is, we need to
      // set the parent to something non-NULL; 'this' works).
      leftParent->Parent() = this;
      leftParent->Left() = NULL;
      leftParent->Right() = NULL;
      delete leftParent;
    }

    // Do we need to delete the right parent?
    if (rightParent != NULL && rightParent != this && rightParent != leftParent)
    {
      // Sever the duplicate parent's children, in the same way as above.
      rightParent->Parent() = this;
      rightParent->Left() = NULL;
      rightParent->Right() = NULL;
      delete rightParent;
    }
  }
}

} // namespace tree
} // namespace mlpack

#endif // MLPACK_CORE_TREE_VANTAGE_POINT_TREE_VANTAGE_POINT_TREE_IMPL_HPP
