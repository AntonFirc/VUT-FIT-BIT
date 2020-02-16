/*
 * Kostra hlavickoveho souboru 3. projekt IZP 2015/16
 * a pro dokumentaci Javadoc.
 */
/**
 * @mainpage
 * Project 3 - Simple cluster analysis. \n
 * Clusters from input file are analyzed and merged together depending on their distance. \n
 * Distance between two clusters is defined by distance of two furthest objects. \n \n
 * To run the application : \n
 * ./proj3 filename [N] \n
 * filename => name of file containing cluster data \n
 * N => final count of clusters
 */
/**
 * obj_t contains data defining cluster object
 */
struct obj_t {
    /// int value to define ID of object
    int id;
    /// float value to define X coordinate of object
    float x;
    /// float value to define Y coordinate of object
    float y;
};
/**
 * cluster_t is cluster containing various objects
 */
struct cluster_t {
    /// defines how many objects does cluster contain
    int size;
    /// defines maximal object capacity of cluster
    int capacity;
    /// pointer to array of objects
    struct obj_t *obj;
};
/**
 * @defgroup list1 Cluster functions
 * @{
 */
/**
 * @defgroup item1 init_cluster
 * @{
 */
 /**
  * Allocates memory for array of objects.
  * @param c pointer to cluster to whom array of objects will be assigned to
  * @param cap desired capacity of cluster <i>(maximal count of objects)</i>
  * @pre Pointer to cluster <i>c</i> can not be equal to NULL.
  * @pre Capacity of cluster <i>cap</i> must be larger than 0.
  * @post If allocation of memory fails, assigns NULL to pointer to array of objects.
  */
void init_cluster(struct cluster_t *c, int cap);
///@}
/**
 * @defgroup item2 clear_cluster
 * @{
 */
/**
 * Removes all objects from cluster.
 * @param c pointer to cluster
 */
void clear_cluster(struct cluster_t *c);
/**
 * @}
 */
/**
 * Value used for reallocation of cluster.
 */
extern const int CLUSTER_CHUNK;
/**
 * @defgroup item3 resize_cluster
 * @{
 */
/**
 * Reallocates memory assigned to cluster to extend capacity.
 * @param  c       cluster to be resized
 * @param  new_cap new capacity of cluster
 * @return         Returns pointer to resized cluster.
 * @pre Pointer to cluster <i>c</i> can not be equal to NULL.
 * @pre Capacity of cluster c must be larger than 0.
 * @pre New capacity of cluster c must be larger than 0.
 * @post If reallocation of memory fails assigns NULL to pointer to array of objects.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap);
/**
 * @}
 */
/**
 * @defgroup item4 append_cluster
 * @{
 */
/**
 * Adds object to the end of a cluster.
 * <i>resizes cluster if needed</i>
 * @param c   cluster to be expanded with object
 * @param obj object that will be added to a cluster
 */
void append_cluster(struct cluster_t *c, struct obj_t obj);
///@}
/**
 * @defgroup item5 merge_clusters
 * @{
 */
/**
 * Adds all objects from <i>cluster 2</i> to <i>cluster 1</i> and sorts all objects in <i>cluster 1</i>.
 * @param c1 <i>cluster 1</i> => to be extended
 * @param c2 <i>cluster 2</i> => to be added to <i>cluster 1</i>
 * @pre Pointer to cluster <i>c1</i> can not be equal to NULL.
 * @pre Pointer to cluster <i>c2</i> can not be equal to NULL.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2);
///@}
///@}
/**
 * @defgroup list2 Array of clusters functions
 * @{
 */
/**
 * @defgroup item6 remove_cluster
 * @{
 */
/**
 * Removes cluster from array of clusters.
 * @param  carr pointer to array of clusters
 * @param  narr count of clusters in array
 * @param  idx  index of cluster to be deleted
 * @return      Returns new count of clusters in array.
 * @pre Index of object to be deleted <i>idx</i> must be smaller than count of objects in array <i>narr</i>.
 * @pre Count of objects in array of clusters <i>narr</i> must be larger than 0.
 */
int remove_cluster(struct cluster_t *carr, int narr, int idx);
///@}
/**
 * @defgroup item7 obj_distance
 * @{
 */
/**
 * Calculates distance of two objects.
 * @param  o1 object 1
 * @param  o2 object 2
 * @return    Returns calculated distance.
 * @pre Pointer to object <i>o1</i> can not be equal to NULL.
 * @pre Pointer to object <i>o2</i> can not be equal to NULL.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2);
///@}
/**
 * @defgroup item8 cluster_distance
 * @{
 */
/**
 * Calculates distance between two clusters. Distance of two clusters is defined by distance between two furthest objects.
 * @param  c1 cluster 1
 * @param  c2 cluster 2
 * @return    Returns the distance between two clusters.
 * @pre Pointer to cluster <i>c1</i> can not be equal to NULL.
 * @pre Size of cluster <i>c1</i> must be larger than 0.
 * @pre Pointer to cluster <i>c2</i> can not be equal to NULL.
 * @pre Size of cluster <i>c2</i> must be larger than 0.
 */
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2);
///@}
/**
 * @defgroup item9 find_neighbours
 * @{
 */
/**
 * Finds two nearest clusters <i>cluster 1 ; cluster 2</i> in array of clusters.
 * @param carr Pointer to array of clusters
 * @param narr Count of clusters in array
 * @param c1 Pointer to return index of cluster 1
 * @param c2 Pointer to return inedx of cluster 2
 * @pre Count of objects in array of clusters must be larger than 0.
 */
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2);
///@}
/**
 * @defgroup item10 sort_cluster
 * @{
 */
/**
 * Sorts all objects in cluster.
 * @param c cluster to be sorted
 */
void sort_cluster(struct cluster_t *c);
///@}
/**
 * @defgroup item11 print_cluster
 * @{
 */
/**
 * Prints ID, X and Y coordinate of all objects in cluster to standard output.
 * @param c cluster whose objects are to be printed
 */
void print_cluster(struct cluster_t *c);
///@}
/**
 * @defgroup item12 load_clusters
 * @{
 */
/**
 * Loads clusters from text file into array of clusters.
 * @param  filename exact name of text file containing cluster data
 * @param  arr      array of clusters
 * @return          Returns count of loaded clusters.
 * @pre Pointer to array of clusters <i>arr</i> can not be equal to NULL.
 * @post All clusters have to be loaded properly, or function assigns NULL to pointer to array of clusters.
 */
int load_clusters(char *filename, struct cluster_t **arr);
///@}
/**
 * @defgroup item13 print_clusters
 * @{
 */
/**
 * Prints first N clusters in array of clusters.
 * @param carr pointer to the first cluster in array of clusters
 * @param narr count of clusters to be printed
 */
void print_clusters(struct cluster_t *carr, int narr);
///@}
///@}
