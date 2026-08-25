# Static pattern rule syntax:
#     targets...: target-pattern: prereq-patterns ...
#         commands
# Explanation
#     target is matched by the target-pattern (via a % wildcard)
#	The matched part is called the "stem".
#         The stem is substituted into the pre-req pattern to generate the
#             target's pre-reqs.
#        

blah bleh bluh : blah% bleh% bluh% :  blah% bleh% bluh%

