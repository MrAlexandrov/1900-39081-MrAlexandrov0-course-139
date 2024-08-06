import pathlib

import pytest

from testsuite.databases.pgsql import discover

import aiohttp


pytest_plugins = ['pytest_userver.plugins.postgresql']

REGISTER_URL = "/register"
LOGIN_URL = "/login"
# request should be with POST method
ADD_BOOKMARK_URL = "/v1/bookmarks"
DELETE_BOOKMARK_URL = "/v1/bookmarks/{id}"
GET_BOOKMARK_URL = "/v1/bookmarks/{id}"
GET_BOOKMARKS_URL = "/v1/bookmarks"

@pytest.fixture(scope='session')
def service_source_dir():
    """Path to root directory service."""
    return pathlib.Path(__file__).parent.parent


@pytest.fixture(scope='session')
def initial_data_path(service_source_dir):
    """Path for find files with data"""
    return [
        service_source_dir / 'postgresql/data',
    ]


@pytest.fixture(scope='session')
def pgsql_local(service_source_dir, pgsql_local_create):
    """Create schemas databases for tests"""
    databases = discover.find_schemas(
        'pg_service_template',  # service name that goes to the DB connection
        [service_source_dir.joinpath('postgresql/schemas')],
    )
    return pgsql_local_create(list(databases.values()))



async def create_multipart_data(email=None, password=None):
    data = aiohttp.MultipartWriter('form-data')
    if email:
        payload = aiohttp.payload.StringPayload(email)
        payload.set_content_disposition('form-data', name='email')
        data.append_payload(payload)
    if password:
        payload = aiohttp.payload.StringPayload(password)
        payload.set_content_disposition('form-data', name='password')
        data.append_payload(payload)
    return data

def create_headers_form_data(data):
    return {
        'Content-Type': f'multipart/form-data; boundary={data.boundary}',
    }

def create_bookmark_data(url, title, tag=None):
    bookmark_data = {
        "url": url,
        "title": title,
    }
    if tag is not None:
        bookmark_data["tag"] = tag
    return bookmark_data


def create_headers_json(user_id):
    return {
        'Content-Type': 'application/json',
        'X-Ya-User-Ticket': user_id
    }


@pytest.fixture
async def register_user(service_client):
    async def register_(email, password):
        data = await create_multipart_data(email=email, password=password)
        headers = create_headers_form_data(data)
        response = await service_client.post(REGISTER_URL, data=data, headers=headers)
        assert response.status == 200
        json_response = response.json()
        assert "id" in json_response
        assert json_response["id"] is not None

        return json_response["id"]
    return register_

@pytest.fixture
async def login_user(service_client):
    async def login_(email, password):
        data = await create_multipart_data(email=email, password=password)
        headers = create_headers_form_data(data)
        response = await service_client.post(LOGIN_URL, data=data, headers=headers)
        assert response.status == 200
        json_response = response.json()
        assert "id" in json_response
        assert json_response["id"] is not None

        return json_response["id"]
    return login_

@pytest.fixture
async def add_bookmark(service_client):
    async def add_(user_id, url, title, tag=None):
        bookmark_data = create_bookmark_data(url, title, tag)
        headers = create_headers_json(user_id)
        response = await service_client.post(ADD_BOOKMARK_URL, json=bookmark_data, headers=headers)
        assert response.status == 200
        json_response = response.json()
        assert "id" in json_response
        return json_response["id"]
    return add_