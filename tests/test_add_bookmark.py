import pytest
from .conftest import ADD_BOOKMARK_URL, create_bookmark_data, create_headers_json


async def test_add_bookmark_missing_url(service_client, register_user, login_user):
    email = "add_bookmark_missing_url@ya.ru"
    password = "add_bookmark_missing_url"
    await register_user(email, password)
    user_id = await login_user(email, password)

    title = "add_bookmark_missing_url"
    tag = "add_bookmark_missing_url"
    bookmark_data = create_bookmark_data(None, title, tag)
    headers = create_headers_json(user_id)

    response = await service_client.post(ADD_BOOKMARK_URL, json=bookmark_data, headers=headers)
    assert response.status == 400

async def test_add_bookmark_missing_title(service_client, register_user, login_user):
    email = "add_bookmark_missing_title@ya.ru"
    password = "add_bookmark_missing_title"
    await register_user(email, password)
    user_id = await login_user(email, password)

    url = "https://add_bookmark_missing_title.ru"
    tag = "add_bookmark_missing_title"
    bookmark_data = create_bookmark_data(url, None, tag)
    headers = create_headers_json(user_id)

    response = await service_client.post(ADD_BOOKMARK_URL, json=bookmark_data, headers=headers)
    assert response.status == 400

async def test_add_bookmark_missing_tag(service_client, register_user, login_user):
    email = "add_bookmark_missing_tag@ya.ru"
    password = "add_bookmark_missing_tag"
    await register_user(email, password)
    user_id = await login_user(email, password)

    url = "https://add_bookmark_missing_tag.ru"
    title = "add_bookmark_missing_tag"
    bookmark_data = create_bookmark_data(url, title, None)
    headers = create_headers_json(user_id)

    response = await service_client.post(ADD_BOOKMARK_URL, json=bookmark_data, headers=headers)
    assert response.status == 200
    json_response = response.json()
    assert "id" in json_response
    assert "url" in json_response
    assert json_response["url"] == url
    assert "title" in json_response
    assert json_response["title"] == title
    assert "created_ts" in json_response

async def test_add_bookmark_same_url(service_client, register_user, login_user):
    email = "add_bookmark_same_url@ya.ru"
    password = "add_bookmark_same_url"
    await register_user(email, password)
    user_id = await login_user(email, password)

    url = "https://add_bookmark_same_url.ru"
    title1 = "First add"
    title2 = "Second add"
    tag1 = "First tag"
    tag2 = "Second tag"

    bookmark_data = create_bookmark_data(url, title1, tag1)
    headers = create_headers_json(user_id)
    response = await service_client.post(ADD_BOOKMARK_URL, json=bookmark_data, headers=headers)
    assert response.status == 200
    json_response = response.json()
    assert "id" in json_response
    assert "url" in json_response
    assert json_response["url"] == url
    assert "title" in json_response
    assert json_response["title"] == title1
    assert "tag" in json_response
    assert json_response["tag"] == tag1
    assert "created_ts" in json_response

    bookmark_data = create_bookmark_data(url, title2, tag2)
    response = await service_client.post(ADD_BOOKMARK_URL, json=bookmark_data, headers=headers)
    assert response.status == 409


async def test_add_bookmark_same_title(service_client, register_user, login_user):
    email = "add_bookmark_same_title@ya.ru"
    password = "add_bookmark_same_title"
    await register_user(email, password)
    user_id = await login_user(email, password)

    url1 = "First url add"
    url2 = "Second url add"
    title = "Yandex"
    tag1 = "First"
    tag2 = "Second"

    assert url1 != url2

    bookmark_data = create_bookmark_data(url1, title, tag1)
    headers = create_headers_json(user_id)
    response = await service_client.post(ADD_BOOKMARK_URL, json=bookmark_data, headers=headers)
    assert response.status == 200
    json_response = response.json()
    assert "id" in json_response
    assert "url" in json_response
    assert json_response["url"] == url1
    assert "title" in json_response
    assert json_response["title"] == title
    assert "tag" in json_response
    assert json_response["tag"] == tag1
    assert "created_ts" in json_response

    bookmark_data = create_bookmark_data(url2, title, tag2)
    response = await service_client.post(ADD_BOOKMARK_URL, json=bookmark_data, headers=headers)
    assert response.status == 200
    json_response = response.json()
    assert "id" in json_response
    assert "url" in json_response
    assert json_response["url"] == url2
    assert "title" in json_response
    assert json_response["title"] == title
    assert "tag" in json_response
    assert json_response["tag"] == tag2
    assert "created_ts" in json_response


async def test_add_bookmark_same_tag(service_client, register_user, login_user):
    email = "add_bookmark_same_tag@ya.ru"
    password = "add_bookmark_same_tag"
    await register_user(email, password)
    user_id = await login_user(email, password)

    url1 = "First url same tag"
    url2 = "Second url same tag"
    title1 = "First Yandex"
    title2 = "Second Yandex"
    tag = "Tag"

    bookmark_data = create_bookmark_data(url1, title1, tag)
    headers = create_headers_json(user_id)
    response = await service_client.post(ADD_BOOKMARK_URL, json=bookmark_data, headers=headers)
    assert response.status == 200
    json_response = response.json()
    assert "id" in json_response
    assert "url" in json_response
    assert json_response["url"] == url1
    assert "title" in json_response
    assert json_response["title"] == title1
    assert "tag" in json_response
    assert json_response["tag"] == tag
    assert "created_ts" in json_response

    bookmark_data = create_bookmark_data(url2, title2, tag)
    response = await service_client.post(ADD_BOOKMARK_URL, json=bookmark_data, headers=headers)
    assert response.status == 200
    json_response = response.json()
    assert "id" in json_response
    assert "url" in json_response
    assert json_response["url"] == url2
    assert "title" in json_response
    assert json_response["title"] == title2
    assert "tag" in json_response
    assert json_response["tag"] == tag
    assert "created_ts" in json_response


async def test_add_bookmark_success(service_client, register_user, login_user):
    email = "add_bookmark_success@ya.ru"
    password = "add_bookmark_success"
    await register_user(email, password)
    user_id = await login_user(email, password)

    url = "add_bookmark_success"
    title = "add_bookmark_success"
    tag = "add_bookmark_success"
    bookmark_data = create_bookmark_data(url, title, tag)
    headers = create_headers_json(user_id)

    response = await service_client.post(ADD_BOOKMARK_URL, json=bookmark_data, headers=headers)
    assert response.status == 200
    json_response = response.json()
    assert "id" in json_response
    assert "url" in json_response
    assert json_response["url"] == url
    assert "title" in json_response
    assert json_response["title"] == title
    assert "tag" in json_response
    assert json_response["tag"] == tag
    assert "created_ts" in json_response
