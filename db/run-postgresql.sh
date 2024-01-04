docker run -d \
  --name wf-security-postgres \
  -p 5432:5432 \
  -e POSTGRES_USER=postgres \
  -e POSTGRES_PASSWORD=doutorado123 \
  -e POSTGRES_DB=WF_SECURITY \
  -e PGDATA=/var/lib/postgresql/data/pgdata \
  -v /home/wsl/git/Wf-Security/db/postgres:/var/lib/postgresql/data \
  postgres

