#!/bin/sh

usage="$(basename "$0") -d <database> -c <cluster> [options] -- Stop a database

Options
-h, --help                  Show this help information 
-d, --database <database>   Database name
-c, --cluster <cluster>     Target cluster"

ec2='aws ec2 --output text'
ssh='ssh -o StrictHostKeyChecking=no'

cluster=
database=

_set_opt()
{
    while [ "$1" != "" ]; do
        case $1 in
        "-h" | "--help")
            echo "$usage"
            exit 0
            ;;
        "-c" | "--cluster")
            shift
            cluster=$1
            ;;
        "-d" | "--database")
            shift
            database=$1
            ;;
        *)
            echo "$usage" >&2
            exit 1
            ;;
        esac
        shift
    done
}

_set_opt $*

if [ "$cluster" = "" ] || [ "$database" = "" ]; then
    echo "$usage" >&2
    exit 1
fi

set -e

# get nodes
query='Reservations[*].Instances[*].[PrivateDnsName]'
nodes=`$ec2 describe-instances --filters "Name=tag:Cluster,Values=$cluster" \
        --query $query`
if [ "$nodes" = "" ]; then
    echo "Could not find cluster \"$cluster\""
    exit 1
fi
set +e

# bring down db on each node
for node in $nodes; do
    $ssh $node $PREFIX'/bin/cdb2sql '$database' local "exec procedure sys.cmd.send(\"exit\")" && while true; do [ `pgrep -a comdb2 | grep -c '$database'` = 0 ] && break; sleep 5; done'
    echo
done
echo OK
