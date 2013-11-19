
class Node:
    def __init__(self, name):
        self.name = name
        self.edges = []
        
    def addEdge(self, node):
        self.edges.append(node)

class DependencyGraph:
    graph = {}
    resolved = []
    unresolved = []
    independant = []
    not_yet_resolved = []
    
    def addNode(self, nodeName):
        if not self.graph.has_key(nodeName):
            self.graph[nodeName] = []
            self.independant.append(nodeName)
            self.not_yet_resolved.append(nodeName)
     
    def addDependency(self, nodeName, dependencyName):
        #if node does not exist, add it
        if not self.graph.has_key(nodeName):
            self.addNode(nodeName)
        #if dependency node does not exist, add it    
        if not self.graph.has_key(dependencyName):
            self.addNode(dependencyName)
        #add dependency to node
        self.graph[nodeName].append(dependencyName)
        #if dependency was independant, it's not anymore remove it from the list
        if self.independant.count(dependencyName):
            self.independant.remove(dependencyName)
            
     
    def getDependencies(self, nodeName):
        if self.graph.has_key(nodeName):
            return self.graph[nodeName]
        return []
  
    def resolve(self, node, l):
        self.unresolved.append(node)
        for edge in self.getDependencies(node):
            if edge not in self.resolved:
                if edge in self.unresolved:
                    print "Circular dependency detected between %s and %s " % (node, edge)
                    return [-1]
                l = self.resolve(edge, l)
            if len(l)==1 and l[0]==-1:
                return [-1]
        self.resolved.append(node)
        self.unresolved.remove(node)
        self.not_yet_resolved.remove(node)
        l.append(node)
        return l
    
    def getNextPath(self):
        self.unresolved = []
        
        if len(self.independant)==0:
            if len(self.resolved)==len(self.graph):
                #all dependencies solved
                return []
            else:
                list = self.resolve(self.not_yet_resolved[0], [])
                return list
        
        list = self.resolve(self.independant.pop(), [])
        return list
        
